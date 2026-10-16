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
  onWait: (kind) => {
    if (!kind) return;
    if (++waits > maxWaits) { rt.closeInput(); return; }
    setImmediate(() => {
      if (kind === "key") { keys++; rt.pressKey(); }
      else if (answers.length) { lines++; const a = answers.shift(); out += `<<${a}>>`; rt.sendLine(a); }
      else { const plain = out.replace(/\x1b\[[0-9;]*[A-Za-z]/g, ""); console.log(plain.slice(-(Number(process.env.TAIL) || 4000))); console.log(`\n--- out of scripted answers; ${keys} key presses, ${lines} lines, ${plain.length} chars`); process.exit(0); }
    });
  },
});

rt.run().then((code) => {
  const plain = out.replace(/\x1b\[[0-9;]*[A-Za-z]/g, "");
  console.log(plain.slice(-(Number(process.env.TAIL) || 4000)));
  console.log(`\n--- exit ${code}; ${keys} key presses, ${lines} lines, ${plain.length} chars of output`);
}).catch((e) => { const plain = out.replace(/\x1b\[[0-9;]*[A-Za-z]/g, ""); console.log(plain.slice(-(Number(process.env.TAIL) || 4000))); console.error("CRASH", String(e), "lines used", lines, "left", answers.length); process.exit(1); });
