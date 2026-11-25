// The player: the song playing, seen three ways on one instrument panel.
//
//   DECK   a piano roll running ahead of the sound (the look-ahead copy of
//          the song: notes arrive from the right and light up as they cross
//          the line), a drum lane under it, each voice's own scope, note and
//          level, and the mix as the PWM gets it.
//   SCOPE  the mix large, with the last frame's trace left behind, and each
//          voice's note, level and pitch in Hz.
//   TRACK  the song as a tracker shows it: a row a sixteenth (or the song's
//          own grid), the four voices' notes and volumes scrolling up through
//          the cursor.
// The status bar names the section the song's cues say it is in, and the
// gauge under it is the way through the loop.
#pragma GCC optimize("Os")
#include "App.h"
#include <string.h>

using namespace agent;

namespace player {

enum View : uint8_t { DECK, SCOPE, TRACK, VIEWS };
static const char *const VIEW_NAMES[VIEWS] = {"DECK", "SCOPE", "TRACKER"};
static const char *const VOICE[4] = {"PUL", "SQR", "SAW", "NSE"};
static uint8_t view = DECK;
static uint16_t t;                      // frames on this screen
static uint8_t shownSong = 0xFF, shownSection = 0;
static int8_t lastMix[128];             // the scope's last trace

void enter() {
    screen = Scr::Player;
    t = 0;
    memset(lastMix, 0, sizeof lastMix);
}

static char *fmtDrive(char *p, uint8_t q) {
    p = fmtInt(p, q / 16);
    uint8_t f = (uint8_t)(q % 16 * 100 / 16);
    if (f) {
        *p++ = '.';
        *p++ = (char)('0' + f / 10);
        if (f % 10) *p++ = (char)('0' + f % 10);
    }
    *p++ = 'X';
    *p = 0;
    return p;
}

// ---- The panel -----------------------------------------------------------------

static void top(const SongInfo &s, uint32_t now) {
    bool paused = ATM.paused();
    statusBar(paused ? "PAUSED" : jukebox::sectionName(), paused ? ALERT : LIVE);
    noteIcon(3, 1, jukebox::hit() < 3 ? PALE : LIVE);
    readout(s.bpm, LIVE, "BP", "M");
    uint32_t pos = now, len = s.intro;
    if (now >= s.intro) {
        pos = (now - s.intro) % s.loop;
        len = s.loop;
    }
    gauge(0, 12, 128, 2, pos, len, MID, GRID);
    // The title, and where we are: the bar and the time.
    tiny(1, 16, s.title, LIVE);
    char buf[16], *p = fmtStr(buf, "B");
    uint32_t bar = now / ((uint32_t)s.beat * s.beats) + 1;
    p = fmtInt(p, (int32_t)bar);
    *p++ = ' ';
    uint8_t rate = ATM.score().tickRate;
    fmtTime(p, (uint16_t)(rate ? now / rate : 0));
    tinyR(127, 16, buf, DIM);
}

static void bottom(const char *hints) {
    keys(121, hints);
    char buf[12], *p = fmtStr(buf, ATM.engine() == atm::BEEP ? "BEEP " : "");
    fmtDrive(p, ATM.drive());
    tinyR(127, 122, buf, ATM.engine() == atm::BEEP ? ALERT : DIM);
}

// ---- DECK ---------------------------------------------------------------------------

static const int ROLL_Y = 23, ROLL_H = 56, NOW_X = 32, DRUM_H = 9;

static int pitchY(const SongInfo &s, uint8_t pitch) {
    int top = ROLL_Y + 2, bottom = ROLL_Y + ROLL_H - DRUM_H - 3;
    int span = s.hi > s.lo ? s.hi - s.lo : 1;
    int p = pitch < s.lo ? s.lo : pitch > s.hi ? s.hi : pitch;
    return bottom - (p - s.lo) * (bottom - top) / span;
}

static void roll(const SongInfo &s, uint32_t now, uint8_t muted) {
    int barT = s.beat * s.beats, drumY = ROLL_Y + ROLL_H - DRUM_H;
    // The grid: bar lines, beats dotted, and the drum lane's floor.
    for (int x = 0; x < 128; x++) {
        int32_t tk = (int32_t)now + (x - NOW_X);
        if (tk < 0) continue;
        if (tk % barT == 0) gfx_vline(x, ROLL_Y, ROLL_H, GRID);
        else if (tk % s.beat == 0)
            for (int y = ROLL_Y + (x & 1); y < ROLL_Y + ROLL_H; y += 3) gfx_pixel(x, y, GRID);
    }
    gfx_hline(0, drumY - 1, 128, GRID);
    // The notes: the past faded, the future arriving, each note's start lit.
    for (int x = 0; x < 128; x++) {
        int32_t tk = (int32_t)now + (x - NOW_X);
        if (tk < 0) continue;
        bool past = x < NOW_X;
        if (past && ((x ^ (int)now) & 1)) continue;
        for (uint8_t ch = 0; ch < 3; ch++) {
            const roll::Cell &c = roll::at((uint32_t)tk, ch);
            uint8_t v = c.vol & 0x7F;
            if (!v || !c.pitch) continue;
            int th = v >= 40 ? 3 : v >= 16 ? 2 : 1;
            int y = pitchY(s, c.pitch);
            uint8_t col = (muted & (1 << ch)) ? GRID : voiceColour(ch, !past && (c.vol & 0x80));
            // An arpeggio or a slide (the pitch moving within one note) is
            // joined up to where it was, every other tick: chords fill out
            // into combed bands.
            const roll::Cell &b = roll::at((uint32_t)tk - 1, ch);
            if (!(c.vol & 0x80) && !(tk & 1) && (b.vol & 0x7F) && b.pitch && b.pitch != c.pitch) {
                int yb = pitchY(s, b.pitch);
                gfx_vline(x, y < yb ? y : yb, (y < yb ? yb - y : y - yb) + 1,
                          (muted & (1 << ch)) ? GRID : voiceColour(ch, false));
            }
            gfx_vline(x, y - th / 2, th, col);
        }
        const roll::Cell &d = roll::at((uint32_t)tk, 3);
        uint8_t v = d.vol & 0x7F;
        if (v) {
            int lane = d.pitch <= 2 ? 6 : d.pitch <= 13 ? 3 : 0;   // kick, snare, the rest
            uint8_t col = (muted & 8) ? GRID : voiceColour(3, (d.vol & 0x80) != 0);
            if (d.vol & 0x80) gfx_fillRect(x, drumY + lane, 2, 2, col);
            else if (v > 3 && !past) gfx_pixel(x, drumY + lane + 1, voiceColour(3, false));
        }
    }
    // The line where the sound is, and a light on every note crossing it.
    gfx_vline(NOW_X, ROLL_Y, ROLL_H, DIM);
    gfx_hline(NOW_X - 2, ROLL_Y, 5, LIVE);
    gfx_hline(NOW_X - 2, ROLL_Y + ROLL_H - 1, 5, LIVE);
    for (uint8_t ch = 0; ch < 3; ch++) {
        const roll::Cell &c = roll::at(now, ch);
        if ((c.vol & 0x7F) && c.pitch && !(muted & (1 << ch))) {
            int y = pitchY(s, c.pitch);
            gfx_fillRect(NOW_X - 1, y - 1, 3, 3, voiceColour(ch, true));
            gfx_pixel(NOW_X, y, PALE);
        }
    }
}

static void strips(const atm::Osc *o, uint8_t muted) {
    const atm::Score &sc = ATM.score();
    for (uint8_t ch = 0; ch < 4; ch++) {
        int x = ch * 32;
        bool on = o[ch].vol && !(muted & (1 << ch));
        uint8_t col = on ? voiceColour(ch, true) : GRID;
        chip(x + 1, 81, 15, VOICE[ch], (muted & (1 << ch)) ? GRID : voiceColour(ch, true));
        char buf[6];
        if (ch == 3) fmtStr(buf, on ? drumName(sc.ch[3].note) : "---");
        else noteName(buf, on ? roll::pitchOf(o[ch].freq) : 0);
        tiny(x + 18, 82, buf, on ? voiceColour(ch, true) : DIM);
        scope::voice(o[ch], ch, x + 1, 90, 30, 12, col);
        uint8_t v = o[ch].vol;
        if (ch == 3) v = (uint8_t)(v * 2);
        gauge(x + 1, 104, 30, 2, v > 63 ? 63 : v, 63, voiceColour(ch, false), GRID);
        if (ch) gfx_vline(x, 81, 25, GRID);
    }
}

static void deck(const SongInfo &s, uint32_t now, const atm::Osc *o, uint8_t muted) {
    roll(s, now, muted);
    strips(o, muted);
    scope::mix(o, muted, 0, 108, 128, 12, PALE, MID, lastMix);
}

// ---- SCOPE ---------------------------------------------------------------------------

static void bigScope(const atm::Osc *o, uint8_t muted) {
    brackets(1, 24, 126, 62, 5, DIM);
    for (int x = 4; x < 124; x += 4) gfx_pixel(x, 55, GRID);
    scope::mix(o, muted, 2, 27, 124, 56, PALE, MID, lastMix);
    for (uint8_t ch = 0; ch < 4; ch++) {
        int y = 89 + ch * 8;
        bool on = o[ch].vol && !(muted & (1 << ch));
        chip(1, y, 15, VOICE[ch], (muted & (1 << ch)) ? GRID : voiceColour(ch, true));
        char buf[12];
        if (ch == 3) fmtStr(buf, on ? drumName(ATM.score().ch[3].note) : "---");
        else noteName(buf, on ? roll::pitchOf(o[ch].freq) : 0);
        tiny(19, y + 1, buf, on ? voiceColour(ch, true) : DIM);
        uint8_t v = o[ch].vol;
        if (ch == 3) v = (uint8_t)(v * 2);
        gauge(34, y + 2, 56, 3, v > 63 ? 63 : v, 63, voiceColour(ch, true), GRID);
        if (ch < 3 && on) {
            char *p = fmtInt(buf, (int32_t)((o[ch].freq + 2) / 4));
            fmtStr(p, "HZ");
        } else {
            fmtStr(buf, ch == 3 && on ? "NOISE" : "");
        }
        tinyR(127, y + 1, buf, on ? voiceColour(ch, false) : DIM);
    }
}

// ---- TRACK ---------------------------------------------------------------------------

static const int TR_TOP = 30, TR_ROWS = 13, ROW_H = 7, CUR_Y = TR_TOP + 6 * ROW_H;
static const int COL_X[4] = {12, 41, 70, 99};

static char hexDigit(uint8_t v) { return (char)(v < 10 ? '0' + v : 'A' + v - 10); }

static void tracker(const SongInfo &s, uint32_t now, uint8_t muted) {
    for (uint8_t ch = 0; ch < 4; ch++)
        tiny(COL_X[ch] + 3, 23, VOICE[ch], (muted & (1 << ch)) ? GRID : voiceColour(ch, true));
    gfx_hline(0, 28, 128, GRID);
    uint8_t n = s.row ? s.row : 4;
    uint32_t row0 = now / n;
    int off = (int)(now % n) * ROW_H / n;
    gfx_fillRect(0, CUR_Y - 1, 128, ROW_H, PANEL);
    brackets(0, CUR_Y - 1, 128, ROW_H, 2, LIVE);
    gfx_setClip(0, TR_TOP - 1, 128, TR_ROWS * ROW_H);
    for (int r = -7; r <= 7; r++) {
        int64_t ri = (int64_t)row0 + r;
        if (ri < 0) continue;
        int y = CUR_Y + r * ROW_H - off;
        uint32_t t0 = (uint32_t)ri * n;
        bool beat = t0 % s.beat == 0;
        char buf[8];
        buf[0] = hexDigit((uint8_t)((ri >> 4) & 15));
        buf[1] = hexDigit((uint8_t)(ri & 15));
        buf[2] = 0;
        tiny(1, y, buf, r == 0 ? PALE : beat ? MID : GRID);
        for (uint8_t ch = 0; ch < 4; ch++) {
            int x = COL_X[ch];
            uint8_t col = (muted & (1 << ch)) ? GRID : voiceColour(ch, true);
            // The first note to start in the row, else whether it sounds.
            const roll::Cell *hitc = nullptr;
            bool sounding = false;
            for (uint8_t k = 0; k < n; k++) {
                const roll::Cell &c = roll::at(t0 + k, ch);
                if (c.vol & 0x80) { hitc = &c; break; }
                if (c.vol) sounding = true;
            }
            if (hitc && (hitc->vol & 0x7F)) {
                if (ch == 3) fmtStr(buf, drumName(hitc->pitch));
                else noteName(buf, hitc->pitch);
                tiny(x, y, buf, col);
                uint8_t v = hitc->vol & 0x7F;
                buf[0] = hexDigit((uint8_t)(v >> 4));
                buf[1] = hexDigit((uint8_t)(v & 15));
                buf[2] = 0;
                tiny(x + 13, y, buf, (muted & (1 << ch)) ? GRID : voiceColour(ch, false));
            } else if (hitc) {
                tiny(x, y, "===", DIM);             // a note-off
            } else {
                tiny(x, y, sounding ? "| " : ". ", sounding ? voiceColour(ch, false) : GRID);
            }
        }
    }
    gfx_resetClip();
}

// ---- The frame ----------------------------------------------------------------------

static void banner(const char *s) { fx::banner(s, fx::B_GREEN, 64, 50); }

void input() {
    if (chgame.justPressed(A_BUTTON)) {
        ATM.playPause();
        sfx(Sfx::Mode);
    }
    if (chgame.justPressed(B_BUTTON) || jukebox::current == 0xFF) {
        sfx(Sfx::Back);
        menu::back();
        return;
    }
    uint8_t cur = jukebox::current;
    if (chgame.justPressed(LEFT_BUTTON)) jukebox::start((uint8_t)((cur + SONG_COUNT - 1) % SONG_COUNT));
    if (chgame.justPressed(RIGHT_BUTTON)) jukebox::start((uint8_t)((cur + 1) % SONG_COUNT));
    int d = chgame.repeat(UP_BUTTON) ? 4 : chgame.repeat(DOWN_BUTTON) ? -4 : 0;
    if (d) {
        int q = ATM.drive() + d;
        q = q < 8 ? 8 : q > 64 ? 64 : q;
        ATM.setDrive((uint8_t)q);
        char buf[16];
        fmtDrive(fmtStr(buf, "DRIVE "), (uint8_t)q);
        banner(buf);
    }
    if (chgame.justPressed(SELECT_BUTTON)) {
        view = (uint8_t)((view + 1) % VIEWS);
        memset(lastMix, 0, sizeof lastMix);
        banner(VIEW_NAMES[view]);
        sfx(Sfx::Move);
    }
    if (chgame.justReleased(START_BUTTON)) {
        ATM.setEngine(ATM.engine() == atm::SYNTH ? atm::BEEP : atm::SYNTH);
        banner(ATM.engine() == atm::BEEP ? "BEEP MODE" : "SYNTH MODE");
    }
}

void draw() {
    if (jukebox::current == 0xFF) return;
    t++;
    const SongInfo &s = SONGS[jukebox::current];
    if (shownSong != jukebox::current) {
        shownSong = jukebox::current;
        shownSection = 0;
        if (t > 1) banner(s.style);
    }
    // The beat on the glass: the grid lines flare on every drum hit, the
    // status bar on every new section.
    if (jukebox::hit() == 0) pal::flash(GRID, 0x527, 4);
    if (jukebox::section != shownSection) {
        shownSection = jukebox::section;
        pal::flash(PANEL, 0x63B, 12);
    }

    uint32_t now = jukebox::now();
    atm::Osc o[4];
    ATM.snapshot(o);
    uint8_t muted = ATM.muted();
    top(s, now);
    if (view == DECK) deck(s, now, o, muted);
    else if (view == SCOPE) bigScope(o, muted);
    else tracker(s, now, muted);
    bottom("A:PAUSE B:LIST SEL:VIEW");
}

char *status(char *p) {
    p = fmtInt(fmtStr(p, "SONG "), jukebox::current == 0xFF ? -1 : jukebox::current);
    p = fmtInt(fmtStr(p, " VIEW "), view);
    p = fmtInt(fmtStr(p, " TICK "), (int32_t)jukebox::now());
    p = fmtInt(fmtStr(p, " PASS "), jukebox::pass());
    p = fmtInt(fmtStr(p, " SECTION "), jukebox::section);
    p = fmtInt(fmtStr(p, " ENGINE "), ATM.engine());
    p = fmtInt(fmtStr(p, " DRIVE "), ATM.drive());
    return fmtInt(fmtStr(p, " PAUSED "), ATM.paused());
}

}  // namespace player
