# Building CHJukebox

How to build the app, its songs, its pictures and its releases from this
repository, and what is where. To just use it, take the cart from a
release: see the [README](README.md#getting-started).

## Prerequisites

- **The CHGame board package, 0.4.0 or later** (Rev1 arrived in 0.4.0),
  in the Arduino IDE or arduino-cli, from
  `https://github.com/bateske/CHGame/releases/latest/download/package_chgame_index.json`.
  It brings the compiler, the uploader and the CHGame and CHGfx libraries:
  enough to build and upload the app.
- **For the songs:** Python 3.10 or later and the `ziglang` package
  (`pip install ziglang`): `make_songs.py` compiles the port for the PC
  with `python -m ziglang c++`.
- **For the simulator, the checks, the cart and the pictures:** a CHGame
  checkout beside this project (`../CHGame`) or named by `CHGAME_ROOT`,
  with its tools installed (`pip install -e .[sim]` in it), which gives the
  `chgame` command, and Pillow.

## Commands

From `CHJukebox/` (the sketch folder):

| What | Command |
|---|---|
| Release build, with its size (the board connected, or `--board rev0` / `--board rev1` first) | `chgame build` |
| Upload it | `chgame upload` |
| Everything that needs no board: the songs, both simulator scripts twice, both boards' builds | `chgame check` |
| A simulator script, screenshots into `out/` | `chgame run tools/scripts/tour.txt out/tour` |
| The README's GIF, `docs/gameplay.gif` (`tools/scripts/gameplay.txt`) | `chgame gif` |
| The cart, `build/CHJukebox.chgame`, with a binary for each board | `chgame export` |
| The songs: `src/songs/*.h` and `Songs.cpp`, each proven (below) | `python tools/songs/make_songs.py` |
| The same, with WAV previews / the notes of every bar | `... make_songs.py --wav out/wav` / `--notes` |

From the repository's root:

| What | Command |
|---|---|
| The banner and the social card, `docs/banner.png` and `docs/social.png` | `chgame --sketch CHJukebox run tools/scripts/brand.txt out/brand` (in `CHJukebox/`), then `python tools/brand.py` |

Without the CHGame tools, arduino-cli builds the release image for either
board:

```bash
arduino-cli compile -b CHGame:ch32v:rev1:opt=oslto,rtlib=nano,periph=game,usb=uploadonly CHJukebox
```

(`rev0` for a Rev0). *Peripherals* must stay **Game**: the synth defines
the timers' interrupt handlers, and *Full* compiles the core's HardwareTimer,
which defines them too.

## The songs

Each song is a Python file in `CHJukebox/tools/songs/` whose `build()`
returns an `atmgen.Song`; `SONGS` in `make_songs.py` is the running order.
`make_songs.py` assembles every song, writes its header in ATM commands,
and proves it with the port itself, built for the PC
(`tools/songs/atmrender.cpp` around `src/atm/ATMlib.cpp`, compiled with
`-DATM_HOST`): the intro and three passes of the loop are played, and

- no channel may stop, and every tick of passes two and three must equal
  the pass before (the loop has no seam);
- the call stack stays within 7, every note within C2..D7;
- the header, compiled against the real `ATMcmds.h`, gives the generator's
  bytes.

It prints each song's size, length, loudness and clipping. The headers in
`src/songs/` are generated: change the Python and run it again.
`NOTES.md` has the assembler's notation and the tricks the songs use.

## The simulator

`chgame run` and `chgame check` drive the app through CHGame's simulator
and its debug protocol (`tools/chsim/chdrive.py` finds CHGame's driver
library through `CHGAME_ROOT` or `../CHGame`). The synth runs there too:
`ATM.pump()` renders it in step with the frames, so the visualiser shows
the real playback, and `$ATM_WAV` names a WAV file to receive it.
The debug command `H` (`state` in a script) reports the screen, the song,
the tick, the section, the engine and the drive.

## A release

1. Bump `JUKE_VERSION` in `CHJukebox/config.h` and commit.
2. `chgame check` in `CHJukebox/` must say ALL GOOD.
3. `chgame export` writes `build/CHJukebox.chgame`: the Rev0 and Rev1
   binaries, the licences and the metadata from `chgame.json` (its picture
   for the visual menu is drawn from the title over the GIF's first frame).
4. Tag `v<version>` and attach the cart to a GitHub release as
   `CHJukebox-<version>.chgame`, with the WAV previews from
   `make_songs.py --wav` beside it.

## What is where

| Path | What |
|---|---|
| `CHJukebox/CHJukebox.ino` | setup, the frame loop, the palette, the debug commands |
| `CHJukebox/Menu.cpp`, `Player.cpp` | the intro and song list; the player's three views and its controls |
| `CHJukebox/Jukebox.cpp`, `Scope.cpp` | the song playing and its look-ahead copy; the waveforms |
| `CHJukebox/Agent.*`, `Fx.*` | the "secret agent" chrome of CHGame's apps; Sizzle's banners |
| `CHJukebox/src/atm/` | the ATMlib port (`ATMcmds.h` is ATMlib's, unchanged) |
| `CHJukebox/src/songs/` | the five songs, generated |
| `CHJukebox/tools/songs/` | the song sources, the assembler, the checks, the PC renderer |
| `CHJukebox/tools/scripts/` | simulator scripts: the tour, the GIF, the screens for the pictures |
| `CHJukebox/tools/game.py`, `tools/chsim/` | what `chgame check` runs; the simulator driver |
| `CHJukebox/README.md`, `NOTES.md` | how to use the app; the port's design, the song briefs, the hardware checklist |
| `tools/brand.py`, `docs/` | the banner and the social card |
