// Packs the game into one self-contained web page.
//   node web/build-page.js
// Produces:
//   dist/index.html     a complete page you can open or host anywhere
//   dist/artifact.html  the same page without the <html>/<head> wrapper
const fs = require("fs");
const path = require("path");
const zlib = require("zlib");

const root = path.join(__dirname, "..");
const read = (p) => fs.readFileSync(path.join(root, p));

const xtermCss = (() => {
  const local = path.join(root, "web/vendor/xterm.css");
  if (fs.existsSync(local)) return fs.readFileSync(local, "utf8");
  throw new Error("web/vendor/xterm.css is missing (copy it from the xterm@5.3.0 npm package)");
})();

const wasmGz = zlib.gzipSync(read("web/mopmania.wasm"), { level: 9 }).toString("base64");
const credits = read("game/endcredit.txt").toString("base64");
const runtime = read("web/mopmania-runtime.js").toString("utf8");

let page = read("web/page-template.html").toString("utf8");
const fill = (marker, value) => {
  if (!page.includes(marker)) throw new Error("template marker missing: " + marker);
  page = page.split(marker).join(value);
};
fill("/*XTERM_CSS*/", xtermCss);
fill("/*RUNTIME*/", runtime);
fill("/*WASM_GZ_B64*/", wasmGz);
fill("/*ENDCREDIT_B64*/", credits);

fs.mkdirSync(path.join(root, "dist"), { recursive: true });
fs.writeFileSync(path.join(root, "dist/artifact.html"), page);
const full =
  '<!doctype html>\n<html lang="en">\n<head>\n<meta charset="utf-8">\n' +
  '<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">\n' +
  "</head>\n<body>\n" + page + "\n</body>\n</html>\n";
fs.writeFileSync(path.join(root, "dist/index.html"), full);
console.log(`dist/index.html ${(full.length / 1024).toFixed(0)} KB`);
