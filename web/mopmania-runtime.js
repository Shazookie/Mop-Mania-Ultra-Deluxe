// Runs the Mop Mania WebAssembly build in a browser (or Node for testing).
//
// The game is ordinary C++ that blocks on std::cin and sleeps between lines.
// The wasm was processed with Binaryen's Asyncify pass, so whenever the game
// waits (reading input, sleeping, "Press any key"), this runtime pauses it,
// lets the page keep running, and resumes it when the wait is over.
(function (root) {
  "use strict";

  const ERRNO = { SUCCESS: 0, BADF: 8, INVAL: 28, NOENT: 44, NOSYS: 52 };
  const FILETYPE = { CHAR: 2, DIR: 3, FILE: 4 };
  const PREOPEN_FD = 3;

  class ExitSignal { constructor(code) { this.code = code; } }

  class MopManiaRuntime {
    /**
     * opts.wasm      ArrayBuffer | Uint8Array | WebAssembly.Module
     * opts.files     { "endcredit.txt": "..." } readable from the game
     * opts.write     (text) => void       terminal output
     * opts.play      (name, loop) => void sound request ("void.wav", ...)
     * opts.stop      () => void
     * opts.onWait    (kind) => void       "line" | "key" | null
     * opts.speed     multiplier for sleeps (1 = original pacing)
     */
    constructor(opts) {
      this.opts = Object.assign({ files: {}, speed: 1, write() {}, play() {}, stop() {}, onWait() {}, lockInput() {} }, opts);
      this.inputLocked = false;
      this.stdin = [];          // queued bytes the game has not read yet
      this.lineWaiter = null;
      this.keyWaiter = null;
      this.sleepCancel = null;
      this.closed = false;
      this.encoder = new TextEncoder();
      this.decoder = new TextDecoder("utf-8");
      this.fds = new Map();
      this.nextFd = PREOPEN_FD + 1;
      this.finished = false;
    }

    // ------------------------------------------------------------ input ---
    get waitingFor() { return this.keyWaiter ? "key" : this.lineWaiter ? "line" : null; }

    /** Give the game a typed line (without the newline). */
    sendLine(text) {
      if (this.inputLocked) return;
      for (const b of this.encoder.encode(text + "\n")) this.stdin.push(b);
      if (this.lineWaiter) { const w = this.lineWaiter; this.lineWaiter = null; w(); }
    }

    /** Answer a "Press any key to continue" prompt. Returns true if one was waiting. */
    pressKey() {
      if (!this.keyWaiter) return false;
      const w = this.keyWaiter; this.keyWaiter = null; w(); return true;
    }

    /** Cut the current pause in the text short (used for "skip"). */
    skipDelay() { if (this.sleepCancel) this.sleepCancel(); }

    /** Make further reads return end-of-file (Node tests). */
    closeInput() {
      this.closed = true;
      if (this.lineWaiter) { const w = this.lineWaiter; this.lineWaiter = null; w(); }
      if (this.keyWaiter) { const w = this.keyWaiter; this.keyWaiter = null; w(); }
    }

    setSpeed(s) { this.opts.speed = s; }

    /** Stop this game for good (used by Restart). It never resumes or prints again. */
    abort() {
      this.dead = true;
      const noop = () => {};
      this.opts.write = noop; this.opts.play = noop; this.opts.onWait = noop;
      try { this.opts.stop(); } catch (e) {}
      this.opts.stop = noop;
      this.lineWaiter = this.keyWaiter = this.sleepCancel = null;
    }

    // --------------------------------------------------------- memory -----
    get mem() { return new DataView(this.memory.buffer); }
    get bytes() { return new Uint8Array(this.memory.buffer); }

    readIovs(iovs, len) {
      const out = [], dv = this.mem;
      for (let i = 0; i < len; i++) {
        out.push({ ptr: dv.getUint32(iovs + i * 8, true), len: dv.getUint32(iovs + i * 8 + 4, true) });
      }
      return out;
    }

    copyIn(iovs, iovsLen, source, nreadPtr) {
      let n = 0;
      const mem = this.bytes;
      for (const { ptr, len } of this.readIovs(iovs, iovsLen)) {
        const take = Math.min(len, source.length - n);
        for (let i = 0; i < take; i++) mem[ptr + i] = source[n + i];
        n += take;
        if (take < len) break;
      }
      this.mem.setUint32(nreadPtr, n, true);
      return n;
    }

    // ----------------------------------------------------------- WASI -----
    wasi() {
      const self = this;
      return {
        args_get: () => ERRNO.SUCCESS,
        args_sizes_get(argc, bufSize) { self.mem.setUint32(argc, 0, true); self.mem.setUint32(bufSize, 0, true); return 0; },
        environ_get: () => ERRNO.SUCCESS,
        environ_sizes_get(count, size) { self.mem.setUint32(count, 0, true); self.mem.setUint32(size, 0, true); return 0; },
        clock_time_get(id, precision, out) {
          const ns = BigInt(Math.round((id === 0 ? Date.now() : performance.now()) * 1e6));
          self.mem.setBigUint64(out, ns, true); return 0;
        },
        clock_res_get(id, out) { self.mem.setBigUint64(out, 1000n, true); return 0; },
        random_get(ptr, len) {
          const buf = self.bytes.subarray(ptr, ptr + len);
          for (let i = 0; i < len; i += 65536) crypto.getRandomValues(buf.subarray(i, Math.min(len, i + 65536)));
          return 0;
        },
        proc_exit(code) { throw new ExitSignal(code); },
        sched_yield: () => 0,

        fd_write(fd, iovs, iovsLen, nwritten) {
          let total = 0;
          if (fd === 1 || fd === 2) {
            let text = "";
            for (const { ptr, len } of self.readIovs(iovs, iovsLen)) {
              text += self.decoder.decode(self.bytes.slice(ptr, ptr + len), { stream: true });
              total += len;
            }
            text = text.replace(/﻿/g, "");
            if (text) self.opts.write(text);
          } else {
            return ERRNO.BADF;
          }
          self.mem.setUint32(nwritten, total, true);
          return 0;
        },

        fd_read(fd, iovs, iovsLen, nread) {
          if (fd === 0) {
            if (self.stdin.length || self.closed) {
              const n = self.copyIn(iovs, iovsLen, self.stdin, nread);
              self.stdin.splice(0, n);
              return 0;
            }
            // Nothing typed yet: pause the game until a line arrives.
            return new Promise((resolve) => {
              self.lineWaiter = resolve;
              self.opts.onWait("line");
            }).then(() => {
              self.opts.onWait(null);
              const n = self.copyIn(iovs, iovsLen, self.stdin, nread);
              self.stdin.splice(0, n);
              return 0;
            });
          }
          const f = self.fds.get(fd);
          if (!f) return ERRNO.BADF;
          const n = self.copyIn(iovs, iovsLen, f.data.subarray(f.pos), nread);
          f.pos += n;
          return 0;
        },

        poll_oneoff(inPtr, outPtr, nsubs, neventsPtr) {
          // The game only ever sleeps on a single clock (nanosleep).
          const dv = self.mem;
          let ms = 0, userdata = 0n;
          for (let i = 0; i < nsubs; i++) {
            const s = inPtr + i * 48;
            if (dv.getUint8(s + 8) === 0) {
              userdata = dv.getBigUint64(s, true);
              ms = Math.max(ms, Number(dv.getBigUint64(s + 24, true)) / 1e6);
            }
          }
          const finish = () => {
            const d = self.mem;
            d.setBigUint64(outPtr, userdata, true);
            d.setUint16(outPtr + 8, 0, true);
            d.setUint8(outPtr + 10, 0);
            d.setUint32(neventsPtr, 1, true);
            return 0;
          };
          const wait = ms * self.opts.speed;
          if (wait < 1) return finish();
          return new Promise((resolve) => {
            const t = setTimeout(done, wait);
            function done() { clearTimeout(t); self.sleepCancel = null; if (!self.dead) resolve(); }
            self.sleepCancel = done;
          }).then(finish);
        },

        fd_close(fd) { return self.fds.delete(fd) ? 0 : (fd <= PREOPEN_FD ? 0 : ERRNO.BADF); },
        fd_fdstat_get(fd, out) {
          const dv = self.mem;
          let type;
          if (fd <= 2) type = FILETYPE.CHAR;
          else if (fd === PREOPEN_FD) type = FILETYPE.DIR;
          else if (self.fds.has(fd)) type = FILETYPE.FILE;
          else return ERRNO.BADF;
          dv.setUint8(out, type);
          dv.setUint16(out + 2, 0, true);
          dv.setBigUint64(out + 8, 0xFFFFFFFFFFFFFFFFn, true);
          dv.setBigUint64(out + 16, 0xFFFFFFFFFFFFFFFFn, true);
          return 0;
        },
        fd_fdstat_set_flags: () => 0,
        fd_prestat_get(fd, out) {
          if (fd !== PREOPEN_FD) return ERRNO.BADF;
          self.mem.setUint8(out, 0);
          self.mem.setUint32(out + 4, 1, true);
          return 0;
        },
        fd_prestat_dir_name(fd, ptr, len) {
          if (fd !== PREOPEN_FD) return ERRNO.BADF;
          self.bytes[ptr] = 46; // "."
          return 0;
        },
        fd_seek(fd, offset, whence, out) {
          const f = self.fds.get(fd);
          if (!f) return fd <= 2 ? ERRNO.INVAL : ERRNO.BADF;
          const off = Number(offset);
          const base = whence === 0 ? 0 : whence === 1 ? f.pos : f.data.length;
          f.pos = Math.max(0, base + off);
          self.mem.setBigUint64(out, BigInt(f.pos), true);
          return 0;
        },
        fd_filestat_get(fd, out) {
          const f = self.fds.get(fd);
          if (!f) return ERRNO.BADF;
          const dv = self.mem;
          for (let i = 0; i < 64; i += 8) dv.setBigUint64(out + i, 0n, true);
          dv.setUint8(out + 16, FILETYPE.FILE);
          dv.setBigUint64(out + 32, BigInt(f.data.length), true);
          return 0;
        },
        path_open(dirfd, dirflags, pathPtr, pathLen, oflags, rb, ri, fdflags, fdOut) {
          if (dirfd !== PREOPEN_FD) return ERRNO.BADF;
          let path = new TextDecoder().decode(self.bytes.slice(pathPtr, pathPtr + pathLen));
          path = path.replace(/^\.\//, "");
          const content = self.opts.files[path];
          if (content == null) return ERRNO.NOENT;
          const data = typeof content === "string" ? self.encoder.encode(content) : new Uint8Array(content);
          const fd = self.nextFd++;
          self.fds.set(fd, { data, pos: 0 });
          self.mem.setUint32(fdOut, fd, true);
          return 0;
        },
        fd_filestat_set_size: () => ERRNO.NOSYS,
        path_filestat_get: () => ERRNO.NOENT,
      };
    }

    game() {
      const self = this;
      const str = (ptr, len) => new TextDecoder().decode(self.bytes.slice(ptr, ptr + len));
      return {
        wait_key() {
          if (self.closed) return;
          return new Promise((resolve) => {
            self.keyWaiter = resolve;
            self.opts.onWait("key");
          }).then(() => { self.opts.onWait(null); });
        },
        play(ptr, len, loop) { try { self.opts.play(str(ptr, len), !!loop); } catch (e) {} },
        stop() { try { self.opts.stop(); } catch (e) {} },
        lock_input(locked) {
          self.inputLocked = !!locked;
          if (!locked) self.stdin.length = 0;   // throw away anything typed while locked
          try { self.opts.lockInput(!!locked); } catch (e) {}
        },
      };
    }

    // ------------------------------------------------------- Asyncify -----
    async run() {
      const NORMAL = 0, UNWINDING = 1, REWINDING = 2;
      let state = NORMAL, pending = null, saved = 0;
      let ex, DATA, DATA_START, DATA_END;
      const self = this;

      const asyncWrap = (fn) => (...args) => {
        if (state === REWINDING) { ex.asyncify_stop_rewind(); state = NORMAL; return saved; }
        const r = fn(...args);
        if (r && typeof r.then === "function") {
          const dv = self.mem;
          dv.setUint32(DATA, DATA_START, true);
          dv.setUint32(DATA + 4, DATA_END, true);
          ex.asyncify_start_unwind(DATA);
          state = UNWINDING;
          pending = r;
          return 0;
        }
        return r;
      };

      const wasi = this.wasi(), game = this.game();
      for (const k of ["fd_read", "poll_oneoff"]) wasi[k] = asyncWrap(wasi[k]);
      game.wait_key = asyncWrap(game.wait_key);

      const imports = { wasi_snapshot_preview1: wasi, mopmania: game };
      const src = this.opts.wasm;
      const instance = src instanceof WebAssembly.Module
        ? await WebAssembly.instantiate(src, imports)
        : (await WebAssembly.instantiate(src, imports)).instance;
      ex = instance.exports;
      this.memory = ex.memory;
      DATA = ex.mm_async_buf();
      DATA_START = this.mem.getUint32(DATA, true);
      DATA_END = this.mem.getUint32(DATA + 4, true);

      let code = 0;
      try {
        ex._start();
        while (state === UNWINDING) {
          ex.asyncify_stop_unwind();
          state = NORMAL;
          saved = await pending;
          pending = null;
          state = REWINDING;
          ex.asyncify_start_rewind(DATA);
          ex._start();
        }
      } catch (e) {
        if (e instanceof ExitSignal) code = e.code;
        else throw e;
      } finally {
        this.finished = true;
        this.opts.onWait(null);
        try { this.opts.stop(); } catch (e) {}
      }
      return code;
    }
  }

  if (typeof module !== "undefined" && module.exports) module.exports = { MopManiaRuntime };
  else root.MopManiaRuntime = MopManiaRuntime;
})(typeof window !== "undefined" ? window : globalThis);
