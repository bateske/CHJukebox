// What the parts of CHJukebox share: the screens, the palette's roles, the
// jukebox (the song playing) and the look-ahead copy of it.
#pragma once
#include <CHGame.h>
#include "config.h"
#include "Agent.h"
#include "Fx.h"
#include "src/atm/ATMlib.h"
#include "src/songs/Songs.h"

// The palette. 0-7 are the secret agent chrome (Agent.h's roles) in the
// jukebox's neon violet; 8-15 are the four voices, a dim and a bright each:
// pulse cyan, square amber, saw pink, noise lime. Every picture of a voice,
// whatever the view, is in its colours.
enum : uint8_t {
    V0_DIM = 8, V0, V1_DIM, V1, V2_DIM, V2, V3_DIM, V3,
};
inline uint8_t voiceColour(int ch, bool bright) { return (uint8_t)(V0_DIM + 2 * ch + (bright ? 1 : 0)); }
extern const uint16_t PALETTE[16];      // RGB444, for pal::init()

extern ATMsynth ATM;

enum class Scr : uint8_t { Intro, List, Player };
extern Scr screen;

// The song playing, and what goes with it: the section its cues name,
// auto-advance, the beat for the LED and the glass.
namespace jukebox {
extern uint8_t current;                 // SONGS[] index, 0xFF: none
extern uint8_t section;                 // the last cue (0: none yet)
extern bool autoNext;                   // move on after two passes of the loop
void start(uint8_t song);               // from the top
void stop();
void update();                          // once a frame: cues, the beat, auto-advance
uint32_t now();                         // the tick sounding (0 before the first)
uint16_t pass();                        // 0 in the intro, then 1, 2 ... each time round the loop
uint8_t hit();                          // frames since the last drum hit (255: long ago)
const char *sectionName();
}

// The look-ahead copy: the same song run in a second score ahead of the
// sound, its voices written down a tick at a time, so the piano roll and
// the tracker can show what is about to play.
namespace roll {
// The piano roll shows 32 ticks behind the sound and 96 ahead; the tracker
// 7 rows each way (56 ticks at its widest grid). RAM is the scarce thing.
static const uint16_t RING = 160;       // ticks kept: the past and the future
static const uint16_t AHEAD = 96;       // ticks run ahead of the sound
struct Cell {
    uint8_t pitch;                      // the nearest note to the voice's frequency (noise: its drum tag); 0: silent
    uint8_t vol;                        // the volume as mixed, bit 7: a note started this tick
};
void begin(const uint8_t *song);
void update(uint32_t now);              // run ahead to now + AHEAD
const Cell &at(uint32_t tick, uint8_t ch);
uint8_t pitchOf(uint16_t freq);         // a note-table frequency -> the nearest note
}

// The voices' waveforms from a snapshot of the oscillators (Scope.cpp).
namespace scope {
// Two cycles of one voice, w x h at (x, y), its height by its volume.
void voice(const atm::Osc &o, uint8_t ch, int x, int y, int w, int h, uint8_t c);
// The mix as the PWM sees it, triggered on the lowest voice so it stands
// still, with the last frame's trace left behind in `trail`.
void mix(const atm::Osc *o, uint8_t muted, int x, int y, int w, int h, uint8_t c, uint8_t trail, int8_t *last);
}

// Each screen takes its input, which may change screens, and then the
// screen showing draws: every frame is drawn and flushed (the simulator's
// lockstep counts frames by their flushes).
namespace menu {
void begin();                           // the intro, then the list
void input();
void draw();
void back();                            // from the player
}

namespace player {
void enter();
void input();
void draw();
char *status(char *p);                  // the H debug command's line
}

// Names: "C-5" / "C#5" (tracker style), a drum's from its tag.
char *noteName(char *p, uint8_t pitch);
const char *drumName(uint8_t tag);
// The status bar's icon: a note with its flag, 7 x 9.
void noteIcon(int x, int y, uint8_t c);
// UI sounds, as ATMlib effects on a voice of their own.
enum class Sfx : uint8_t { Move, Open, Back, Mode };
void sfx(Sfx s);
