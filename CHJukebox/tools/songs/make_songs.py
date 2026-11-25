"""Write CHJukebox's songs: src/songs/<song>.h and src/songs/Songs.cpp.

    python tools/songs/make_songs.py [--wav DIR] [--notes] [SONG ...]

Each song is a Python file beside this one (its build() returns an
atmgen.Song). For every song this assembles the bytes, writes the header in
ATM_* commands, and then proves it with the port itself (atmrender.cpp,
built with zig): the song is played for its intro and three passes of its
loop, and
  - no channel may stop (a looping song never does),
  - every tick of the second and third pass must equal the pass before it
    (each voice's volume, and its frequency while it sounds),
  - the call stack stays within 7 and every note within C2..D7,
  - the header, compiled against the real ATMcmds.h, gives the same bytes.
It prints each song's size, length, loudness and clipping; --wav writes a
preview of each (intro + two passes, 16-bit, 15,625 Hz: the board's own
samples), --notes the notes of every bar for checking the harmony by eye.
"""
import argparse
import importlib
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
APP = HERE.parents[1]
SRC = APP / "src" / "songs"
BUILD = APP / "build" / "host"
sys.path.insert(0, str(HERE))
import atmgen  # noqa: E402

# The jukebox's running order.
SONGS = ["invention", "jig", "funk", "jungle", "nocturne"]


def zig(args):
    r = subprocess.run([sys.executable, "-m", "ziglang", "c++", "-std=gnu++17"] + args, capture_output=True, text=True)
    if r.returncode:
        sys.exit(r.stderr)


def renderer():
    exe = BUILD / "atmrender.exe"
    srcs = [HERE / "atmrender.cpp", APP / "src" / "atm" / "ATMlib.cpp", APP / "src" / "atm" / "ATMlib.h"]
    if not exe.exists() or any(s.stat().st_mtime > exe.stat().st_mtime for s in srcs):
        BUILD.mkdir(parents=True, exist_ok=True)
        zig(["-O2", "-DATM_HOST", str(srcs[0]), str(srcs[1]), "-o", str(exe)])
    return exe


def check_header(song, hfile):
    cpp = BUILD / f"{song.ident}_dump.cpp"
    cpp.write_text(f'#include <stdio.h>\n#include <stdint.h>\n#include "{hfile.as_posix()}"\n'
                   f'int main(int, char **v) {{ FILE *f = fopen(v[1], "wb"); fwrite({song.ident}, 1, sizeof {song.ident}, f); '
                   f'fclose(f); return 0; }}\n')
    exe = BUILD / f"{song.ident}_dump.exe"
    zig(["-w", str(cpp), "-o", str(exe)])
    out = BUILD / f"{song.ident}_dump.bin"
    subprocess.run([str(exe), str(out)], check=True)
    if out.read_bytes() != song.data:
        sys.exit(f"{song.ident}: the compiled header differs from the generator's bytes")


def verify(song, exe, wav=None):
    loop = song.length - song.loop_at
    ticks = song.loop_at + 3 * loop
    secs = ticks / song.tempo + 0.5
    binf = BUILD / f"{song.ident}.bin"
    binf.write_bytes(song.data)
    tl = BUILD / f"{song.ident}.timeline"
    r = subprocess.run([str(exe), str(binf), f"{secs:.2f}", str(BUILD / f"{song.ident}.wav"), str(tl)],
                       capture_output=True, text=True, check=True)
    stats = dict(zip(r.stdout.split()[0::2], r.stdout.split()[1::2]))
    rows = {}
    for line in tl.read_text().splitlines():
        p = line.split()
        rows[int(p[0])] = ([tuple(int(x) for x in c.split(",")) for c in p[2:6]], int(p[7]))
    problems = []
    table = [0, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494] + [0] * 51
    for i in range(13, 64):
        table[i] = table[i - 12] * 2 if i < 61 else [8372, 8870, 9397][i - 61]
    pitches = set()
    for chans, _ in rows.values():
        for k in range(3):
            if chans[k][2] and chans[k][1]:
                f = chans[k][1]
                pitches.add(min(range(1, 64), key=lambda n: abs(table[n] - f)))
    song.lo, song.hi = (min(pitches), max(pitches)) if pitches else (1, 63)
    used = sum(1 << i for i in range(4) if song.used[i])
    for t in range(ticks):
        if t not in rows:
            problems.append(f"no tick {t}")
            break
        chans, active = rows[t]
        if active & used != used:
            problems.append(f"a channel stopped at tick {t} (active {active:04b})")
            break
        for k, c in enumerate(chans):
            if c[4] > 7:
                problems.append(f"ch{k} call depth {c[4]} at tick {t}")
            if k < 3 and c[0] and (c[0] > 63 or (c[1] == 0 and c[2])):
                problems.append(f"ch{k} note {c[0]} out of range at tick {t}")
        if t >= song.loop_at + loop:
            prev = rows[t - loop][0]
            for k in range(4):
                a, b = chans[k], prev[k]
                if a[2] != b[2] or (a[2] and k < 3 and a[1] != b[1]):
                    problems.append(f"ch{k} pass {(t - song.loop_at) // loop + 1} differs at tick {t}: {b[:3]} -> {a[:3]}")
                    break
        if len(problems) > 5:
            break
    if wav:
        out = Path(wav) / f"{song.ident}.wav"
        out.parent.mkdir(parents=True, exist_ok=True)
        secs2 = (song.loop_at + 2 * loop) / song.tempo + 1
        subprocess.run([str(exe), str(binf), f"{secs2:.2f}", str(out), "-"], check=True, capture_output=True)
    return problems, stats


def notes_by_bar(song):
    """Each channel's note-ons per bar, for checking harmony by eye."""
    names = ["C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"]
    for p in song.parts:
        if not song.used[p.ch]:
            continue
        print(f"  ch{p.ch}:")
        bars = {}
        tra = 0
        for t, x in p.items:
            if isinstance(x, atmgen.Cmd) and x.b[0] == 0x4C:
                tra = x.b[1] - 256 if x.b[1] > 127 else x.b[1]
            if isinstance(x, atmgen.Cmd) and x.kind == "note" and x.b[0]:
                n = x.b[0] + (tra if p.ch < 3 else 0) - 1
                bars.setdefault(t // song.bar, []).append(f"{names[n % 12]}{2 + n // 12}")
        for b in sorted(bars):
            print(f"    {b + 1:3d}: {' '.join(bars[b])}")


def cstr(s):
    return '"' + s.replace('"', '\\"') + '"'


def write_table(songs):
    h = SRC / "Songs.h"
    h.write_text("""// The jukebox's songs. Written by tools/songs/make_songs.py: do not edit by hand.
#pragma once
#include <stdint.h>

struct SongInfo {
    const uint8_t *data;        // the ATMlib song
    uint16_t bytes;
    const char *title;          // upper case, for the 5x7 font
    const char *style;          // the genre, a chip's worth
    const char *key;
    const char *blurb;          // one line about it
    uint8_t bpm;
    uint8_t beat;               // ticks a beat
    uint8_t beats;              // beats a bar
    uint8_t sectionCount;
    uint8_t row;                // ticks a tracker row (a sixteenth, or the song's own grid)
    uint8_t lo, hi;             // the lowest and highest note its tonal voices sound
    uint16_t intro;             // ticks before the loop
    uint16_t loop;              // ticks in the loop
    const char *const *sections;    // ATM_CUE(n) starts sections[n - 1]
};

extern const SongInfo SONGS[];
static const uint8_t SONG_COUNT = %d;
""" % len(songs), encoding="utf-8", newline="\n")
    lines = ["// The jukebox's songs. Written by tools/songs/make_songs.py: do not edit by hand.",
             '#include "Songs.h"']
    for s in songs:
        lines.append(f'#include "{s.ident}.h"')
    lines.append("")
    for s in songs:
        lines.append(f"static const char *const {s.ident}Sections[] = {{"
                     + ", ".join(cstr(n) for _, n in s.sections) + "};")
    lines.append("")
    lines.append("const SongInfo SONGS[] = {")
    for s in songs:
        lines.append(f"    {{{s.ident}, sizeof {s.ident}, {cstr(s.title)}, {cstr(s.style)}, {cstr(s.key)},")
        lines.append(f"     {cstr(s.blurb)}, {round(s.bpm())}, {s.beat}, {s.beats}, {len(s.sections)}, "
                     f"{s.row}, {s.lo}, {s.hi}, {s.loop_at}, {s.length - s.loop_at}, {s.ident}Sections}},")
    lines.append("};")
    (SRC / "Songs.cpp").write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("songs", nargs="*")
    ap.add_argument("--wav", help="write a preview WAV of each song into this folder")
    ap.add_argument("--notes", action="store_true", help="print the notes of every bar")
    a = ap.parse_args()
    exe = renderer()
    SRC.mkdir(parents=True, exist_ok=True)
    built = []
    bad = False
    for name in SONGS:
        if not (HERE / f"{name}.py").exists():
            continue
        mod = importlib.import_module(name)
        song = mod.build()
        song.blurb = getattr(song, "blurb", "")
        song.row = getattr(song, "row", song.beat // 4)
        if a.songs and name not in a.songs:
            song.assemble()
            verify(song, exe)
            built.append(song)
            continue
        song.assemble()
        h = SRC / f"{song.ident}.h"
        h.write_text(song.header(), encoding="utf-8", newline="\n")
        check_header(song, h)
        problems, st = verify(song, exe, a.wav)
        loop = song.length - song.loop_at
        secs = song.length / song.tempo
        print(f"{song.ident:10s} {len(song.data):5d} B  {len(song.tracks):3d} tracks  {song.loop_at // song.bar:2d}+"
              f"{loop // song.bar:2d} bars  {secs:5.1f} s  {song.bpm():5.1f} BPM  peak {st['peak']:>3s}  "
              f"clipped {int(st['clipped']) * 100 / max(1, int(st['samples'])):4.1f}%  rms {st['rms']}"
              + ("  OK" if not problems else ""))
        for p in problems:
            print("   ", p)
            bad = True
        if a.notes:
            notes_by_bar(song)
        built.append(song)
    write_table(built)
    print(f"total {sum(len(s.data) for s in built)} B in {len(built)} songs")
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
