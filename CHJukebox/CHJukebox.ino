// CHJukebox - ATMlib, the Arduboy's four-voice tracker music, ported to the
// CHGame handheld (CH32X035 at 48 MHz, a 128x128 ST7735, one buzzer pin),
// with five songs and a visualiser.
//
// The synth (src/atm) mixes a pulse, a square, a saw and a noise voice
// 15,625 times a second in the buzzer's timer interrupt and plays the mix as
// an 8-bit PWM at 187.5 kHz; songs are the Arduboy's own byte format. It
// asks the board package where the buzzer and the LED are, so the same
// source builds for Rev0 and Rev1: pick the board, build.
// The visualiser runs a second copy of the song ahead of the sound, so the
// piano roll shows the notes before they play.
//
// The art style is the apps' "secret agent" (Agent.h, shared with the other
// apps): a spy's wristwatch LCD, here in a jukebox's neon violet, the four
// voices each in a colour of their own.
//
//   List      UP / DOWN      pick a song (hold to repeat)
//             A              play it (or open the player on the one playing)
//             B              stop the music
//             START          SYNTH (the PWM mix) / BEEP (square waves, the
//                            CHGame library's way: for a buzzer that does not
//                            take to the mix)
//   Player    A              pause / play
//             B              back to the list (the music plays on)
//             LEFT / RIGHT   the song before / after
//             UP / DOWN      DRIVE: the mix's gain before it clips, x0.5-x4
//             SELECT         the view: DECK / SCOPE / TRACKER
//             START          SYNTH / BEEP
//             START (hold)   held 3 s: the SD game menu
// Every song plays its intro and two passes of its loop, then the next one
// starts: left alone, it is a jukebox.
//
// The files:
//   CHJukebox.ino   setup, the frame loop, the palette, the debug commands
//   Menu.cpp        the intro and the song list
//   Player.cpp      the player's three views and its controls
//   Jukebox.cpp     the song playing, the look-ahead copy, the UI sounds
//   Scope.cpp       the waveforms, from the oscillators
//   Agent.*         the secret agent chrome (shared with the other apps)
//   src/atm         the ATMlib port (BSD-3-Clause, LICENSE-ATMlib)
//   src/songs       the songs, written by tools/songs/make_songs.py
#include <CHGame.h>
#include "config.h"
#include "App.h"

// The palette (RGB444). 0-7: the secret agent chrome (Agent.h's roles), the
// glass a violet-black and the readings a jukebox's neon violet; 8-15: the
// voices, dim then bright: pulse cyan, square amber, saw pink, noise lime.
// FX_A/FX_B cycling is off: those slots are the noise voice's.
const uint16_t PALETTE[16] = {
    0x102,          // BG     the glass
    0xFDF,          // PALE   the brightest text
    0x214,          // GRID   rules, off segments, empty gauges
    0x63B,          // MID    the selected tab, a gauge's fill
    0xC7F,          // LIVE   live readings
    0x748,          // DIM    labels
    0xF35,          // ALERT  paused, BEEP
    0x204,          // PANEL  the status bar, the highlight
    0x077, 0x5FF,   // pulse
    0x740, 0xFC3,   // square
    0x803, 0xF5A,   // saw
    0x360, 0xAF4,   // noise
};

ATMsynth ATM;
Scr screen = Scr::Intro;

#if CHGAME_DEBUG
// H: the state, for scripts. P (the library's) has the perf figures.
static bool debugHook(char cmd, const char *) {
    if (cmd != 'H') return false;
    char buf[120], *p = fmtStr(buf, "STATE SCREEN ");
    p = fmtInt(p, (int32_t)screen);
    p = player::status(fmtStr(p, " "));
    fmtStr(p, "\n");
    dbg::print(buf);
    return true;
}
#endif

void setup() {
    chgame.boot();
    dbg::begin("JUKE " JUKE_VERSION);
    gfx_begin(GFX_DIV2, GFX_12BPP);
    pal::init(PALETTE);
    pal::setCycling(false);
    chgame.setFrameRate(JUKE_FPS);
#if CHGAME_DEBUG
    dbg::hook = debugHook;
#endif
    menu::begin();
}

void loop() {
    dbg::poll();
    // The simulator renders the synth up to the frame's time; the board's
    // interrupt needs nothing.
    ATM.pump(chgame.frameCount * (1000000u / JUKE_FPS));
    if (!chgame.nextFrame()) return;
    chgame.pollButtons();
    jukebox::update();
    if (screen == Scr::Player) player::input();
    else menu::input();
    fx::update();
    pal::tick();

    gfx_wait();
    pal::commit();
    gfx_clear(agent::BG);
    if (screen == Scr::Player) player::draw();
    else menu::draw();
    fx::drawBanner();
    gfx_flushAsync();
}
