# CHJukebox: notes

For someone changing the app or the port. The README says what it is.

## The port (src/atm)

`ATMlib.h` / `ATMlib.cpp` are T-arg/ATMlib's `ATMlib.h` / `ATMlibimpl.h`,
with `ATMcmds.h` copied unchanged, so any ATMlib song plays as written.

**The playroutine is the Arduboy's, statement for statement**, every effect
and function compiled in (there is no `ATMconfig.h`). It was checked against
ATMlib's own Python model, `extras/tools/atmsim.py`: all twelve of ATMlib's
example songs (Quest Theme, Italo Night, Cutlass Cove, which uses nearly
every effect, Orbital Rush ...) give the same volume and frequency on every
channel on every tick for 40 seconds, with no difference anywhere. Its quirks
are kept on purpose, because songs were written against them: a volume
slide is not clamped at 63, an arpeggio adds the transposition twice, the
glissando counter is not reset, a restart clears the mutes.

What differs, and why:

| | Arduboy | CHGame |
|---|---|---|
| Output | Timer4 PWM on the piezo, mixed in its overflow interrupt | The buzzer's timer channel (from the board package: below): an 8-bit PWM at 187.5 kHz (ARR 255 at 48 MHz), the repetition counter (RPTCR 11) raising the update interrupt every 12th period |
| Sample rate | ~15.7 kHz in practice (Arduino's init leaves Timer4 in phase-correct mode, which halves ATMlib's 31.25 kHz; that is why "ticks a second = tempo" was measured on hardware) | 15,625 Hz exactly |
| Tuning | The table read as ~0.24 Hz units: every note ~75 cents flat | The oscillators step `freq x 68719` through a 32-bit phase, so the table reads as quarter-Hz: A4 = 440 Hz. Slides and vibrato keep the table's units |
| Tempo | A tick every `15625 / tempo` samples, rounded down | Kept as a fraction: exactly `tempo` ticks a second |
| Silence | A voice at note 0 holds +volume (DC), which clicks | A tonal voice is silent unless its note is on and its frequency above ~8 Hz (`atm::gate`) |
| The mix | 8-bit sum, wraps past 255 | Summed in full, scaled by DRIVE (x1.5 by default), clipped. The rest level rises to half way in 16 ms when sound starts and falls to 0 (the pin low) in 33 ms after a quarter of a second of silence, a pause or a stop: the buzzer carries no DC while silent |
| Long delay | `read_vle()` steps its own argument, not the song pointer: after an `ATM_LONG_DELAY` the delay's byte runs as a command (none of ATMlib's songs use it) | Reads it as the README describes |
| Safety | none | A call past the 7-deep stack, a track the song does not have, or 64 commands without a delay stops the channel (it would otherwise run wild in an interrupt) |

**The interrupt.** The buzzer timer's update interrupt writes the sample
computed last time into the channel's compare register (preloaded: it
lands on the next update, so every sample is exactly one period late) and
then computes the next one; the playroutine
runs in the same interrupt when a tick is due. `sample()`, `tick()`,
`step()` and the helpers are RAMFUNCs (about 2.3 KB of SRAM): from flash a
tick of four channels could outlast the 64 us between samples. The mix is
about 6% of the CPU; a tick about 40 us, 30-46 times a second.

**One source, every board.** The hardware part of `ATMlib.cpp` names no
pin and no timer. It asks the board package: the variant's `PIN_BUZZER`,
looked up in the variant's own `PinMap_TIM` (the table `analogWrite()`
uses) with the core's `pinmap_peripheral()` / `pinmap_function()`, gives the
timer, the channel and the remap, and `pin_function()` puts the pin on it;
`PIN_LED` is the beat's LED. The board picked (*Tools > Board*, or `chgame
--board rev0|rev1`) is the only difference between the binaries:

| | Rev0 | Rev1 |
|---|---|---|
| Buzzer | PB10, TIM1 CH2 (partial remap 1): a piezo | PA1, TIM2 CH2 (remap 000): a MOSFET (AO3400A) under a magnetic buzzer (MLT-5020) fed from VSYS through 22 ohms, a 1N5819 across it |
| LED | PB9 | PB10 |
| Backlight | always on | PB9, CHGfx's: never touched here |

TIM1 and TIM2 are advanced timers on the CH32X035 (a repetition counter,
MOE). A timer with no repetition counter (it reads back 0) gets a fallback:
one PWM period a sample, 3,072 counts. Handlers are defined for TIM1, TIM2
and TIM3; only the buzzer's has its interrupt enabled. Before the pin goes
to the timer its latch is set low and the channel forced low, so the pin is
never high on the way: on Rev1 a high PA1 is current through the coil.

**The first version was Rev0's alone**: it wrote TIM1, PB10 and PB9 directly.
Built for Rev1 it compiled and ran, but drove Rev0's LED pin, which is
Rev1's backlight (forced off when sound started, lit for three frames on
every drum hit: the screen strobed), played the mix on Rev1's LED, and left
the buzzer alone. Any sketch with its own register-level sound or LED code
does the same; going through the variant's names is what makes the board
menu enough.

**Peripherals.** It owns the buzzer's timer and pin (as the CHGame
library's `audio::` does: do not use both) and the SysTick hook
`osSystickHandler` (for BEEP). It needs *Peripherals: Game* (the default):
with *Full* the core's HardwareTimer defines the same interrupt handlers.
CHGame's `Audio.cpp` is never linked (nothing calls `audio::`), so its
`osSystickHandler` does not clash.

**BEEP** is the fallback for a buzzer that does not take to the PWM mix
(the earlier CHGameSound library's DMA synth "did not work well" on a board,
cause unknown; see the hardware checklist). In SysTick's 1 kHz hook it ticks
the score and puts one voice on the pin as a square wave, the CHGame
library's way: the tonal voices take 6 ms turns, a drum hit takes the pin
for 12 ms of random pitches, a volume becomes a pulse width, notes under
400 Hz go up in octaves.

**For the visualiser** the port exposes the score (`ATM.score()`), the
oscillators (`snapshot()`), a tick count, the mix's peak, and
`atm::Score` / `atm::scoreTick()`, a whole second playroutine with nothing
muted. `Chan` has two fields the original has not: `trig` (a note started
this tick) and `fx` (the last effect command).

**In the simulator** `ATM.pump(us)` renders the samples up to the frame's
time (frames at 60 a second, so a lockstep run keeps the music in time),
and `$ATM_WAV` names a WAV file to get them.

## The app

- **Every frame is drawn and flushed**: input first (which may change the
  screen), then the screen showing draws. A frame that flushes nothing
  stalls the simulator's lockstep (it counts frames by their flushes):
  the first version returned early on a screen change and every scripted
  run hung or died there.
- **The look-ahead** (`Jukebox.cpp`, `roll::`): a second score runs the
  song 96 ticks ahead of the sound and writes a cell a tick a voice (the
  nearest note to its frequency, so arpeggios and slides show; the noise
  voice's note, which it ignores, is the drum's tag; the volume; whether a
  note started). The ring keeps 160 ticks: the roll shows 32 behind, the
  tracker at most 56. RAM was 768 B short of comfortable with 256.
- **Drum tags**: the songs write each hit on the noise voice with a note
  the noise ignores: 1 kick, 3 snare, 4 ghost, 13/25 the clock, 38 brush,
  42 hat, 46 open hat, 49 riser, 51 ride (`drumName()`). The roll puts
  them in lanes, the tracker names them.
- **Sections** are the songs' `ATM_CUE`s (`make_songs.py` writes cue n at
  section n); the status bar shows the name, and flashes on a new one.
- **The beat**: a jump in the noise voice's volume flashes the grid colour
  and lights the LED for three frames.
- **UI sounds** are ATMlib effects (`ATM.playSfx` on CH0): over a song
  they take the pulse voice for a moment and give it back.
- **Palette**: 0-7 the agent chrome's roles in violet, 8-15 the voices,
  dim then bright. FX_A/FX_B cycling is off (those are the noise voice's).
- Debug: `H` gives `STATE SCREEN s SONG n VIEW v TICK t PASS p SECTION c
  ENGINE e DRIVE d PAUSED x` (`tools/chsim/chdrive.py`'s `state`).

## The songs (tools/songs)

`make_songs.py` writes `src/songs/<song>.h` (ATM commands, the header in
hex, as ATMlib's README asks) and `Songs.cpp` (the table: title, style,
key, tempo, grid, note range, loop, section names), and proves every song
with the port itself (`atmrender.cpp`, built with zig): intro and three
passes of the loop played, no channel stopping, every tick of passes two
and three equal to the pass before (the hop-free self-loop of ATMlib's
README, section 6), the call stack within 7, notes within C2..D7, and the
header compiled against ATMcmds.h giving the generator's bytes.
`--wav DIR` writes previews (16-bit, 15,625 Hz: the board's own samples),
`--notes` the notes of every bar.

`atmgen.py` is the assembler: a part is commands and waits; it cuts each
channel into bars, gives a bar that comes back a track (a REPEAT for a run)
when that saves bytes, writes the rest inline, makes track 0 a dummy
nobody calls (the README's track-0 trap) and loops each channel's body
track on itself. `music.py` has the scales, diatonic moves, a drum kit for
the noise voice (hits as tag + VOL + SL_VOL/NOISE when they change, state
tracked per bar so equal bars share), and a two-voice counterpoint check
(the invention's only strong-beat dissonance is the E diminished chord's
own tritone).

| Song | Bytes | Bars | Length | Tempo |
|---|---|---|---|---|
| Clockwork Invention | 848 | 16 | 34 s | 30 (112 BPM) |
| The Brass Kettle | 785 | 2 + 32 | 35 s | 35 (117 BPM, 18 ticks a dotted quarter) |
| Velvet Strut | 1,500 | 2 + 24 | 57 s | 29 (109 BPM) |
| Circuit Breaker | 1,217 | 4 + 20 | 33 s | 46 (172 BPM) |
| Rain on Glass | 739 | 2 + 16 | 58 s | 30 (75 BPM, 24 ticks a beat: swing is 16 + 8) |

Each was written to a brief, as a songwriter would be given one:

### Clockwork Invention (baroque)

"A Bach-style two-part invention in D minor at 112 BPM for a clockwork
harpsichord. Strict two-voice counterpoint: a one-bar subject in running
sixteenths (a scale that climbs to the leading tone and snaps back down)
stated alone in the right hand, answered at the octave in the left while
the right hand plays a countersubject in steady eighths. A descending
circle-of-fifths episode (Dm Gm C F Bb E-dim A Dm) built from one figure
moved down a step every bar, a middle entry in the relative major (F) in
both voices, a second episode with the roles swapped (the left hand runs,
the right hand sings a falling scale), then the final entries, the subject
an octave higher at the climax, a dominant pedal on A where both hands run
in tenths, and a cadence onto a D major chord (a Picardy third). Every note
a pluck that dies away. No drums: a quiet clock ticks the beats."

### The Brass Kettle (Celtic jig)

"A rollicking Irish double jig in 6/8 at 117 dotted-quarter beats a minute,
in D major with a mixolydian turn (C natural) in the high part. A fiddle
tune of two 8-bar parts played AABB, the way a session plays it: the lilt
of a jig is in its 'D-D-D' figures and its runs; the second time through
each part the fiddler ornaments it, with cuts (a flick of a higher note
before a repeated one), a roll on the long notes and slides up into the
high notes. Under it a drone of open fifths like uilleann pipes, swelling
slowly; a bouzouki-style bass that bounces root and fifth on the beat; and
a bodhran: a deep stroke on each beat, two lighter ones after it, and a
rolled fill at the end of every part. Two bars of drone and drum to count
it in."

### Velvet Strut (disco-funk)

"Late-70s disco-funk at 109 BPM in E dorian. The bass is the star: a
slap-style line on the saw that thumbs the root on the one, pops the
octave, and walks up chromatically (G, G#, A) into the third beat. A
'chicken-scratch' guitar on the square: muted sixteenth chops, each chord a
fast three-note arpeggio in a real funk voicing (Em9 as B-D-F#, A13 as the
quartal C#-F#-B, Cmaj7, Bm7, Am7, and a Hendrix B7#9 for the turnaround).
Four-on-the-floor disco drums on the noise channel: a thudding kick on
every beat, a crack on 2 and 4, open hats on the off-beats, fills every
fourth bar. The pulse plays a sassy, bluesy hook with slides up into notes
and vibrato on the held ones, answered by the bass. Form: two bars of drums
and guitar to count it in, then the groove (Em9 - A13, the lead only
answering), the hook over Cmaj7 - Bm7 - Am7 - B7#9, a breakdown where only
bass and hats are left, and the hook again to bring it home."

### Circuit Breaker (drum & bass)

"Mid-90s jungle at 172 BPM in F minor, the liquid kind. The noise channel
is the star: a chopped breakbeat in the spirit of the Amen, with ghost
snares between the hits, a kick that thuds, a 32nd-note snare roll and a
break edit every fourth bar. Under it a 'reese' bass on the saw: long,
sustained notes with a slow vibrato for that detuned wobble. The square
plays a dreamy pad of slow three-note arpeggios with tremolo over Fm7 -
Dbmaj7 - Bbm7 - C7, and the pulse a sparse lead whose every note is echoed
twice, quieter, a dotted eighth apart: a dub delay made of notes. Form:
four bars of intro (pad, hats and ghost notes, the lead far away, a noise
riser into the drop), the drop (eight bars), a half-time switch-up where
the bass dives, and the second drop with the lead an octave up."

### Rain on Glass (lo-fi jazz)

"A late-night lo-fi jazz ballad at 75 BPM with a lazy triplet swing, in Db
major. Lush extended harmony played as slow rolled arpeggios, each chord
voiced by its colour tones above the bass: Dbmaj9 (Ab-C-Eb), Bbm9, Ebm9,
Ab13 (Gb-C-F), Gbmaj7, then the bittersweet backdoor Gbm6, Fm7 to Bb7b9,
and Ebm9 to D7, the tritone substitute that slides home to Db. A walking
bass, a quarter a beat, chord tones on the strong beats and chromatic
approach notes into every change. The pulse sings a muted-trumpet melody,
laid back behind the beat, held notes with a slow vibrato, scoops up into
the high ones, a blue note in the second chorus and a climb to a high Bb at
its peak. Brushes: a swish on 2 and 4, a swung ride pattern, and the faint
crackle of a record. Form: two bars of intro, the 8-bar tune, and the tune
again, embellished and higher."

Tricks the briefs needed, for the next songwriter:
- **Chord voicings in one voice**: `ATM_ARP` plays three notes, so a chord
  is its colour tones (Ab-C-Eb over a Db bass is Dbmaj9; Gb-C-F over Ab is
  Ab13), the bass supplying the root.
- **Ornaments**: a cut is a 1-tick grace note; a roll a five-note turn; a
  slide a note a tone low with `ATM_GLIS(0x01)` for 4 ticks (a multiple of
  its period, so every loop is the same).
- **Echoes**: the same note again at lower `ATM_VOL`s a dotted eighth apart.
- **Drums that thud**: `ATM_NOISE` retriggered every tick makes the noise
  periodic at the tick rate, a low buzz; a hit with it on is NOTE, then VOL
  (a note-on would load the retrigger counter as the volume).
- **Swing**: 24 ticks a beat, 16 + 8.

## On the board: the checklist

Heard on a Rev0 (2026-10-21): SYNTH works and sounds as the previews do,
but quietly (the piezo on one pin is the limit); BEEP is louder and rougher.
Rev1 (the MOSFET-driven magnetic buzzer) not yet. What to listen for, in
order:
1. **Any sound** on the first song in SYNTH. Silence with the visualiser
   running means the timer path (remap, MOE, the repetition counter, the
   update interrupt); START for BEEP proves the buzzer and the rest.
2. **Timing**: the tempo in the status bar against a metronome; the clock
   in Clockwork Invention ticks on the beat. Drift means the sample rate is
   not 15,625 Hz (RPTCR).
3. **Loudness**: UP/DOWN for DRIVE. If x1.5 is too quiet and higher
   distorts, the buzzer is the limit (Rev0: one pin, no amplifier).
4. **Rev1: the screen stays lit** and the LED (PB10) flashes on the drums.
   A dark or strobing screen means something drives PB9 (the backlight).
5. **Carrier whine or hiss** at rest: there should be none (the rest level
   is 0 when nothing plays, 50% only while a song runs).
6. **Clicks during drawing**: the audio interrupt and CHGfx's DMA interrupt
   share a priority; if samples are late the mix crackles on busy frames.
   The fix would be the buzzer timer's interrupt above DMA1 channel 3's.
7. BEEP mode on every song (the voices taking turns).

## Sizes

Release builds (board package 0.4.1): image 33,120 B (Rev0) / 33,188 B
(Rev1) of 50,944; static RAM 17,000 B of 18,416
(the framebuffer 8 KB, CHGfx's buffers 2 KB, the look-ahead ring 1,280 B,
the port's SRAM functions about 2.3 KB). It saves nothing (`SAVE_PAGES = 0`).

## Open items

- Box art (`tools/cart.py`, `docs/cart.png`) for the visual menu.
- The interrupt's priority over CHGfx's DMA, if the board crackles.
- Channel mutes in the UI (`ATM.muteChannel()` is there).
- Promote `src/atm` to a library in the board package, for games.
