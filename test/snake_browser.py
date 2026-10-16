# Test-only: plays into Putrid's snake game in headless Chromium and takes screenshots.
import asyncio, sys
from playwright.async_api import async_playwright
ROOT = "/home/claude/mopmania-ultra-deluxe"
NPM = "/home/claude/tools/npm"
OUT = sys.argv[1]
LOCAL = {
  "https://cdn.jsdelivr.net/npm/xterm@5.3.0/lib/xterm.js": f"{NPM}/xterm-5.3.0/package/lib/xterm.js",
  "https://cdn.jsdelivr.net/npm/xterm-addon-fit@0.8.0/lib/xterm-addon-fit.js": f"{NPM}/xterm-addon-fit-0.8.0/package/lib/xterm-addon-fit.js",
}
async def route(r):
    u = r.request.url
    if u in LOCAL: await r.fulfill(path=LOCAL[u], content_type="application/javascript")
    elif u.startswith("file://"): await r.continue_()
    else: await r.abort()
ANSWERS = ["y","1","b","m","1","l","l","l","b","l","3"]
async def run(p, name, vp, mobile):
    b = await p.chromium.launch()
    ctx = await b.new_context(viewport=vp, is_mobile=mobile, has_touch=mobile, device_scale_factor=2 if mobile else 1)
    page = await ctx.new_page()
    errs = []
    page.on("pageerror", lambda e: errs.append(str(e)))
    await page.route("**/*", route)
    await page.goto(f"file://{ROOT}/dist/index.html")
    await page.select_option("#speed", "0")
    await page.wait_for_timeout(1500)
    answers = list(ANSWERS)
    async def answer(t):
        if mobile: await page.fill("#touch-text", t); await page.click("#touch-send")
        else: await page.keyboard.type(t); await page.keyboard.press("Enter")
    async def key():
        if mobile: await page.click("#touch-send")
        else: await page.keyboard.press("Space")
    # get to the snake game
    for _ in range(600):
        if not await page.is_hidden("#pad"): break
        s = await page.get_attribute("#status", "data-state")
        if s == "key": await key()
        elif s == "line" and answers: await answer(answers.pop(0))
        await page.wait_for_timeout(100)
    print(name, "snake started:", not await page.is_hidden("#pad"))
    # steer: down, then left, a few times
    moves = ["Down","Left","Up","Right"] * 3
    for i, m in enumerate(moves):
        await page.wait_for_timeout(2600 if i == 0 else 450)
        if mobile: await page.dispatch_event(f'#pad button[aria-label="{m}"]', "pointerdown")
        else: await page.keyboard.press("Arrow" + m)
        if i == 1: await page.wait_for_timeout(250); await page.screenshot(path=f"{OUT}/{name}-snake.png")
    for _ in range(100):
        if await page.is_hidden("#pad"): break
        await page.wait_for_timeout(200)
    await page.wait_for_timeout(1500)
    await page.screenshot(path=f"{OUT}/{name}-after.png")
    txt = await page.evaluate("document.querySelector('.xterm-rows').innerText")
    print(name, "after:", " | ".join(l.strip() for l in txt.splitlines() if l.strip())[:300])
    print(name, "errors:", errs)
    await b.close()
async def main():
    async with async_playwright() as p:
        await run(p, "desktop", {"width":1280,"height":800}, False)
        await run(p, "phone", {"width":390,"height":780}, True)
asyncio.run(main())
