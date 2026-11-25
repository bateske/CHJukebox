// The intro and the song list. The list is the jukebox's front: every song
// with its style on a chip, its key and tempo, the one playing marked by a
// little equaliser that moves with its voices, and the highlighted one's
// liner note running along the bottom like a ticker. Browsing does not stop
// the music; A plays the song highlighted (or opens the player on the one
// playing), B stops it.
#pragma GCC optimize("Os")
#include "App.h"
#include <string.h>

using namespace agent;

namespace menu {

static const int LIST_TOP = 17, ROW_H = 19;
static uint16_t t;                      // frames on this screen
static uint8_t sel;
static int barY = -1;                   // the highlight's row on screen, x4 (it slides)

void begin() {
    screen = Scr::Intro;
    t = 0;
}

void back() {
    screen = Scr::List;
    t = 0;
    if (jukebox::current != 0xFF) sel = jukebox::current;
}

// Four bars in the voices' colours: heights from their levels, or a dance.
static void equaliser(int x, int y, int h, bool live) {
    atm::Osc o[4];
    ATM.snapshot(o);
    for (int ch = 0; ch < 4; ch++) {
        int v;
        if (live) {
            v = ch == 3 ? o[3].vol * 2 : o[ch].vol;
            v = v * h / 48;
        } else {
            v = (fx::isin((uint8_t)(t * (5 + ch * 2) + ch * 60)) + 256) * h / 512;
        }
        if (v > h) v = h;
        if (v < 1) v = 1;
        gfx_fillRect(x + ch * 3, y + h - v, 2, v, voiceColour(ch, true));
    }
}

// ---- The intro -----------------------------------------------------------------------

static void introInput() {
    if (chgame.justPressed(A_BUTTON | B_BUTTON | START_BUTTON) || t >= 150) {
        screen = Scr::List;
        t = 0;
        return;
    }
    if (t == 40) sfx(Sfx::Open);
}

static void introDraw() {
    if (t < 40) {
        statusBar("STANDBY", DIM);
        noteIcon(3, 1, DIM);
        readout(t * 100 / 39, LIVE, "%", "");
        brackets(4, 20, 120, 100, 6, GRID);
        sweep(14 + t * 3);
    } else {
        statusBar("ONLINE", LIVE);
        noteIcon(3, 1, LIVE);
        readout(100, LIVE, "%", "");
        brackets(4, 20, 120, 100, 6, DIM);
        int n = (t - 40) / 2;
        for (int ch = 0; ch < 4; ch++) {            // four voices, each in its colour, dancing
            int x = 26 + ch * 20;
            int h = (fx::isin((uint8_t)(t * (4 + ch) + ch * 64)) + 256) * 22 / 512 + 2;
            gfx_fillRect(x, 50 - h, 14, h, voiceColour(ch, false));
            gfx_fillRect(x, 50 - h, 14, 2, voiceColour(ch, true));
        }
        outlined(56, "JUKEBOX", LIVE, BG);
        typed(22, 80, "ATMLIB  4 VOICES", n, DIM, 1, n < 16 && (t & 8));
        if (n > 18) centred(94, "5 SONGS LOADED", PALE);
        if (n > 26 && (t & 16)) centred(108, "PRESS A", MID);
    }
}

// ---- The list ---------------------------------------------------------------------------

static void listInput() {
    if (chgame.repeat(UP_BUTTON) && sel > 0) { sel--; sfx(Sfx::Move); t = 0; }
    if (chgame.repeat(DOWN_BUTTON) && sel + 1 < SONG_COUNT) { sel++; sfx(Sfx::Move); t = 0; }
    if (chgame.justPressed(A_BUTTON)) {
        if (jukebox::current != sel) jukebox::start(sel);
        sfx(Sfx::Open);
        player::enter();
        return;
    }
    if (chgame.justPressed(B_BUTTON) && jukebox::current != 0xFF) {
        jukebox::stop();
        sfx(Sfx::Back);
    }
    if (chgame.justReleased(START_BUTTON)) {
        ATM.setEngine(ATM.engine() == atm::SYNTH ? atm::BEEP : atm::SYNTH);
        fx::banner(ATM.engine() == atm::BEEP ? "BEEP MODE" : "SYNTH MODE", fx::B_GREEN, 64, 50);
    }
    int target = (LIST_TOP + sel * ROW_H) * 4;
    if (barY < 0) barY = target;
    barY += (target - barY + (target > barY ? 2 : -2)) / 3;
}

static void listDraw() {
    bool playing = jukebox::current != 0xFF;
    statusBar(playing ? "PLAYING" : "JUKEBOX", LIVE);
    noteIcon(3, 1, playing && jukebox::hit() < 3 ? PALE : LIVE);
    readout(SONG_COUNT, LIVE, "TR", "KS");
    gauge(0, 12, 128, 2, sel + 1u, SONG_COUNT, MID, GRID);

    int by = barY / 4;
    gfx_fillRect(0, by, 128, ROW_H - 1, PANEL);
    brackets(0, by, 128, ROW_H - 1, 3, LIVE);
    for (uint8_t i = 0; i < SONG_COUNT; i++) {
        const SongInfo &s = SONGS[i];
        int y = LIST_TOP + i * ROW_H;
        bool on = i == sel;
        uint8_t c = voiceColour(i % 4, true);
        int w = tinyW(s.style) + 5;
        chip(3, y + 2, w, s.style, on ? c : voiceColour(i % 4, false));
        char buf[24], *p = fmtStr(buf, s.key);
        p = fmtStr(p, "  ");
        p = fmtInt(p, s.bpm);
        fmtStr(p, " BPM");
        tinyR(124, y + 3, buf, on ? LIVE : DIM);
        gfx_text(3, y + 10, s.title, on ? PALE : MID);
        if (i == jukebox::current) equaliser(112, y + 10, 7, true);
    }

    // The highlighted song's liner note, as a ticker.
    const SongInfo &s = SONGS[sel];
    char line[96], *p = fmtStr(line, s.blurb);
    p = fmtStr(p, "  -  ");
    p = fmtInt(p, s.bytes);
    p = fmtStr(p, " BYTES  -  ");
    gfx_hline(0, 113, 128, GRID);
    int wl = tinyW(line) + 4;
    int sx = -(int)((t / 2) % (uint16_t)wl);
    gfx_setClip(0, 114, 128, 6);
    tiny(sx, 115, line, DIM);
    tiny(sx + wl, 115, line, DIM);
    gfx_resetClip();

    keys(121, sel == jukebox::current ? "A:OPEN B:STOP" : playing ? "A:PLAY B:STOP" : "A:PLAY");
    tinyR(127, 122, ATM.engine() == atm::BEEP ? "BEEP" : "SYNTH", ATM.engine() == atm::BEEP ? ALERT : DIM);
}

void input() {
    t++;
    if (screen == Scr::Intro) introInput();
    else listInput();
}

void draw() {
    if (screen == Scr::Intro) introDraw();
    else listDraw();
}

}  // namespace menu
