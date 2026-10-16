// Headless smoke test: drives the web build of Mop Mania with scripted input.
// Usage: node test/play.js "1,2,3,..."   (line answers; key presses are automatic)
const fs = require("fs");
const path = require("path");
const { MopManiaRuntime } = require("../web/mopmania-runtime.js");

const answers = (process.argv[2] || "").split(",").filter((s) => s !== "");
const maxWaits = Number(process.argv[3] || 400);
const root = path.join(__dirname, "..");
let out = "", waits = 0, keys = 0, lines = 0;

const rt = new MopManiaRuntime({
  wasm: fs.readFileSync(path.join(root, "web/mopmania.wasm")),
  files: { "endcredit.txt": fs.readFileSync(path.join(root, "game/endcredit.txt"), "utf8") },
  speed: 0,
  write: (t) => { out += t; },
  play: (name, loop) => { out += `\n[sound ${name}${loop ? " (loop)" : ""}]\n`; },
  stop: () => {},
  // Title screen (real-time, Enter only). TITLE=skip presses Enter at once;
  // otherwise Enter is pressed after the "PRESS ENTER" idle loop has shown twice.
  onRealtime: (on, mode) => {
    if (!on || mode !== 1) return;
    if (process.env.TITLE === "skip") { setTimeout(() => rt.sendKeys("\r"), 50); return; }
    const t = setInterval(() => {
      if ((out.match(/P R E S S   E N T E R/g) || []).length >= (Number(process.env.LOOPS) || 2)) { clearInterval(t); rt.sendKeys("\r"); }
    }, 50);
  },
  onWait: (kind) => {
    if (!kind) return;
    if (++waits > maxWaits) { rt.closeInput(); return; }
    setImmediate(() => {
      if (kind === "key") { keys++; rt.pressKey(); }
      else if (answers.length) { lines++; const a = answers.shift(); out += `<<${a}>>`; rt.sendLine(a); }
      else { if (process.env.DUMP) fs.writeFileSync(process.env.DUMP, out); const plain = out.replace(/\x1b\[[0-9;]*[A-Za-z]/g, ""); console.log(plain.slice(-(Number(process.env.TAIL) || 4000))); console.log(`\n--- out of scripted answers; ${keys} key presses, ${lines} lines, ${plain.length} chars`); process.exit(0); }
    });
  },
});

rt.run().then((code) => {
  if (process.env.DUMP) fs.writeFileSync(process.env.DUMP, out);
  const plain = out.replace(/\x1b\[[0-9;]*[A-Za-z]/g, "");
  console.log(plain.slice(-(Number(process.env.TAIL) || 4000)));
  console.log(`\n--- exit ${code}; ${keys} key presses, ${lines} lines, ${plain.length} chars of output`);
}).catch((e) => { const plain = out.replace(/\x1b\[[0-9;]*[A-Za-z]/g, ""); console.log(plain.slice(-(Number(process.env.TAIL) || 4000))); console.error("CRASH", String(e), "lines used", lines, "left", answers.length); process.exit(1); });
