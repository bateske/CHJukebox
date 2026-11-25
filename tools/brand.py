"""The README's banner and the repository's social card, from the app itself.

    python tools/brand.py

writes docs/banner.png (1280 x 352) and docs/social.png (1280 x 640, for
GitHub's Settings > Social preview). Everything in them is the app's own:
its palette, CHGfx's 5x7 font (the status bar's), simulator screenshots of
its screens, and, behind it all, a song's real notes drawn as a faint piano
roll. It needs, from the steps in BUILDING.md:

  - CHJukebox/out/brand/*.png        chgame run tools/scripts/brand.txt out/brand
  - CHJukebox/build/host/*.timeline  python CHJukebox/tools/songs/make_songs.py
  - CHGfx's font: a CHGame checkout ($CHGAME_ROOT, or ../CHGame), or the
    installed board package.
"""
import os
import re
from pathlib import Path

from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[1]
APP = ROOT / "CHJukebox"


def rgb(c444):
    return tuple(((c444 >> s) & 15) * 17 for s in (8, 4, 0))


# CHJukebox.ino's PALETTE: the chrome's roles, then the voices dim / bright.
BG, PALE, GRID, MID, LIVE, DIM, ALERT, PANEL = (rgb(c) for c in (0x102, 0xFDF, 0x214, 0x63B, 0xC7F, 0x748, 0xF35, 0x204))
VOICE = [(rgb(0x077), rgb(0x5FF)), (rgb(0x740), rgb(0xFC3)), (rgb(0x803), rgb(0xF5A)), (rgb(0x360), rgb(0xAF4))]


def font5x7():
    """CHGfx's chgfx_font5x7: 95 glyphs, 5 columns, bit 0 the top row."""
    cands = []
    if os.environ.get("CHGAME_ROOT"):
        cands.append(Path(os.environ["CHGAME_ROOT"]))
    cands.append(ROOT.parent / "CHGame")
    cands = [c / "platform/board/arduino/CHGame/libraries/CHGfx/src/CHGfx_font.h" for c in cands]
    pkg = Path(os.environ.get("LOCALAPPDATA", Path.home())) / "Arduino15/packages/CHGame/hardware/ch32v"
    cands += sorted(pkg.glob("*/libraries/CHGfx/src/CHGfx_font.h"), reverse=True)
    cands += sorted((Path.home() / ".arduino15/packages/CHGame/hardware/ch32v").glob("*/libraries/CHGfx/src/CHGfx_font.h"),
                    reverse=True)
    for c in cands:
        if c.exists():
            body = c.read_text(encoding="utf-8").split("{", 1)[1].split("}", 1)[0]
            body = re.sub(r"/\*.*?\*/", "", body, flags=re.S)
            data = [int(x, 16) for x in re.findall(r"0x[0-9A-Fa-f]+", body)]
            return {chr(32 + i): data[i * 5:i * 5 + 5] for i in range(95)}
    raise SystemExit("brand.py: CHGfx_font.h not found (set CHGAME_ROOT to a CHGame checkout)")


FONT = font5x7()


def text_mask(s):
    """The text as a 1-bit grid (6 columns a character, 7 rows)."""
    w = len(s) * 6 - 1
    img = Image.new("1", (w, 7), 0)
    px = img.load()
    for i, ch in enumerate(s):
        for x, col in enumerate(FONT.get(ch, FONT["?"])):
            for y in range(7):
                if col >> y & 1:
                    px[i * 6 + x, y] = 1
    return img


def blit_text(im, x, y, s, scale, colour):
    m = text_mask(s).resize((len(s) * 6 * scale - scale, 7 * scale), Image.NEAREST)
    im.paste(colour, (x, y), m)
    return m.size


def title(im, x, y, s, scale):
    """The app's outlined lettering, larger: a ring in the glass, a violet
    shadow, a fill falling from pale to the live violet."""
    m = text_mask(s).resize((len(s) * 6 * scale - scale, 7 * scale), Image.NEAREST)
    ring = scale // 2 + 2
    for dx in range(-ring, ring + 1, max(1, ring // 2)):
        for dy in range(-ring, ring + 1, max(1, ring // 2)):
            im.paste(BG, (x + dx, y + dy), m)
    im.paste(MID, (x + scale // 2, y + scale // 2 + scale // 2), m)
    fill = Image.new("RGB", m.size)
    d = ImageDraw.Draw(fill)
    for row in range(7):                    # a step a font row: the pixel art's own bands
        t = row / 6
        c = tuple(int(PALE[k] + (LIVE[k] - PALE[k]) * t) for k in range(3))
        d.rectangle([0, row * scale, m.size[0], (row + 1) * scale - 1], fill=c)
    im.paste(fill, (x, y), m)
    return m.size


def chip(im, x, y, s, scale, colour):
    w = (len(s) * 6 - 1) * scale + 4 * scale
    h = 9 * scale
    ImageDraw.Draw(im).rectangle([x, y, x + w - 1, y + h - 1], fill=colour)
    blit_text(im, x + 2 * scale, y + scale, s, scale, BG)
    return w


def brackets(d, x0, y0, x1, y1, arm, width, colour):
    for (cx, cy, sx, sy) in ((x0, y0, 1, 1), (x1, y0, -1, 1), (x0, y1, 1, -1), (x1, y1, -1, -1)):
        d.rectangle(sorted([cx, cx + sx * arm]) and [min(cx, cx + sx * arm), min(cy, cy + sy * (width - 1)),
                    max(cx, cx + sx * arm), max(cy, cy + sy * (width - 1))], fill=colour)
        d.rectangle([min(cx, cx + sx * (width - 1)), min(cy, cy + sy * arm),
                     max(cx, cx + sx * (width - 1)), max(cy, cy + sy * arm)], fill=colour)


def screen(name):
    """A screenshot from the brand script, at the panel's own 128 x 128."""
    p = APP / "out" / "brand" / f"{name}.png"
    if not p.exists():
        raise SystemExit(f"brand.py: {p} is missing: run tools/scripts/brand.txt first (BUILDING.md)")
    im = Image.open(p).convert("RGB")
    return im.resize((128, 128), Image.NEAREST)


def device(im, shot, x, y, scale):
    """A screenshot as the handheld's glass: a bezel, the app's brackets."""
    s = shot.resize((128 * scale, 128 * scale), Image.NEAREST)
    d = ImageDraw.Draw(im)
    pad = 3 * scale
    d.rectangle([x - pad - 2, y - pad - 2, x + s.width + pad + 1, y + s.height + pad + 1], fill=(8, 0, 16))
    d.rectangle([x - pad, y - pad, x + s.width + pad - 1, y + s.height + pad - 1], fill=PANEL)
    brackets(d, x - pad, y - pad, x + s.width + pad - 1, y + s.height + pad - 1, 6 * scale, max(2, scale), LIVE)
    im.paste(s, (x, y))


def piano_roll(im, song, px_tick, offset, dim=0.42):
    """A song's real notes, from make_songs' timeline, as a faint roll."""
    tl = APP / "build" / "host" / f"{song}.timeline"
    if not tl.exists():
        raise SystemExit(f"brand.py: {tl} is missing: run make_songs.py first (BUILDING.md)")
    rows = [line.split() for line in tl.read_text().splitlines()]
    notes = []
    for r in rows:
        t = int(r[0])
        for ch in range(3):
            n, f, v = (int(x) for x in r[2 + ch].split(",")[:3])
            if v and f:
                notes.append((t, ch, n, v))
    lo = min(n for _, _, n, _ in notes)
    hi = max(n for _, _, n, _ in notes)
    W, H = im.size
    d = ImageDraw.Draw(im)
    for x in range(0, W, px_tick * 16):     # beats, dotted
        for y in range(0, H, 6):
            d.rectangle([x, y, x + 1, y + 1], fill=GRID)
    span = H / (hi - lo + 4)
    for t, ch, n, v in notes:
        x = (t - offset) * px_tick
        if not 0 <= x < W:
            continue
        y = int(H - (n - lo + 2) * span)
        c = tuple(int(BG[k] + (VOICE[ch][1][k] - BG[k]) * dim * (0.5 + v / 126)) for k in range(3))
        d.rectangle([x, y, x + px_tick - 1, y + max(2, int(span * 0.6))], fill=c)


def social():
    W, H = 1280, 640
    im = Image.new("RGB", (W, H), BG)
    piano_roll(im, "funk", 4, 700, dim=0.5)
    shade = Image.new("RGB", (700, H), BG)
    im.paste(Image.blend(im.crop((0, 0, 700, H)), shade, 0.55), (0, 0))
    device(im, screen("deck_funk"), 716, 64, 4)
    tw, th = title(im, 56, 84, "CHJUKEBOX", 11)
    y = 84 + th + 44
    for line, colour in (("ATMLIB TRACKER MUSIC", PALE), ("FOR THE CHGAME", PALE), ("4 VOICES  5 SONGS", LIVE),
                         ("A LIVE VISUALIZER", LIVE)):
        blit_text(im, 60, y, line, 5, colour)
        y += 7 * 5 + 18
    x = 60
    for i, name in enumerate(("PUL", "SQR", "SAW", "NSE")):
        x += chip(im, x, y + 10, name, 4, VOICE[i][1]) + 16
    return im


def banner():
    W, H = 1280, 352
    im = Image.new("RGB", (W, H), BG)
    piano_roll(im, "invention", 3, 130, dim=0.5)
    shade = Image.new("RGB", (W, H), BG)
    im = Image.blend(im, shade, 0.35)
    device(im, screen("tracker_jungle"), 724, 48, 2)
    device(im, screen("deck_jungle"), 1000, 48, 2)
    tw, th = title(im, 48, 64, "CHJUKEBOX", 8)
    blit_text(im, 52, 64 + th + 34, "ATMLIB ON THE CHGAME", 4, PALE)
    blit_text(im, 52, 64 + th + 34 + 44, "4-VOICE CHIPTUNE, 5 SONGS", 4, LIVE)
    x = 52
    for i, name in enumerate(("PUL", "SQR", "SAW", "NSE")):
        x += chip(im, x, 64 + th + 34 + 96, name, 3, VOICE[i][1]) + 12
    return im


def main():
    out = ROOT / "docs"
    out.mkdir(exist_ok=True)
    banner().save(out / "banner.png", optimize=True)
    social().save(out / "social.png", optimize=True)
    for n in ("banner.png", "social.png"):
        im = Image.open(out / n)
        print(f"docs/{n}: {im.width} x {im.height}, {(out / n).stat().st_size:,} B")


if __name__ == "__main__":
    main()
