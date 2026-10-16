import asyncio, sys, os
from playwright.async_api import async_playwright
ROOT = sys.argv[2] if len(sys.argv) > 2 else "/home/claude/mopmania"
NPM = "/home/claude/tools/npm"
OUT = sys.argv[1]
LOCAL = {
  "https://cdn.jsdelivr.net/npm/xterm@5.3.0/lib/xterm.js": f"{NPM}/xterm-5.3.0/package/lib/xterm.js",
  "https://cdn.jsdelivr.net/npm/xterm-addon-fit@0.8.0/lib/xterm-addon-fit.js": f"{NPM}/xterm-addon-fit-0.8.0/package/lib/xterm-addon-fit.js",
}
async def route(r):
    u = r.request.url
    if u in LOCAL:
        await r.fulfill(path=LOCAL[u], content_type="application/javascript")
    elif u.startswith("file://"):
        await r.continue_()
    else:
        await r.abort()
async def run(p, name, vp, mobile):
    b = await p.chromium.launch()
    ctx = await b.new_context(viewport=vp, is_mobile=mobile, has_touch=mobile, device_scale_factor=2 if mobile else 1)
    page = await ctx.new_page()
    errs = []
    page.on("console", lambda m: m.type == "error" and errs.append(m.text))
    page.on("pageerror", lambda e: errs.append(str(e)))
    await page.route("**/*", route)
    await page.goto(f"file://{ROOT}/dist/index.html")
    await page.select_option("#speed", "0.4")
    await page.wait_for_timeout(4000)
    await page.screenshot(path=f"{OUT}/{name}-1-title.png")
    status = lambda: page.inner_text("#status")
    print(name, "status at title:", await status())
    async def answer(text):
        if mobile:
            await page.fill("#touch-text", text); await page.click("#touch-send")
        else:
            await page.keyboard.type(text); await page.keyboard.press("Enter")
    async def key():
        if mobile: await page.click("#touch-send")
        else: await page.keyboard.press("Space")
    await key()
    for want, val in [("line","y"),("line","1")]:
        for _ in range(60):
            s = await page.get_attribute("#status","data-state")
            if s == "key": await key()
            elif s == want: break
            await page.wait_for_timeout(250)
        await answer(val)
    for _ in range(80):
        s = await page.get_attribute("#status","data-state")
        if s == "key": await key()
        elif s == "line": break
        await page.wait_for_timeout(250)
    await page.screenshot(path=f"{OUT}/{name}-2-play.png")
    print(name, "status later:", await status())
    sw = await page.evaluate("document.documentElement.scrollWidth > window.innerWidth")
    print(name, "horizontal overflow:", sw, "errors:", errs)
    await b.close()
async def main():
    async with async_playwright() as p:
        await run(p, "desktop", {"width":1280,"height":800}, False)
        await run(p, "phone", {"width":390,"height":780}, True)
asyncio.run(main())
