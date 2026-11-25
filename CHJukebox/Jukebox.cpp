// The jukebox (the song playing, its cues, the beat, moving on) and the
// look-ahead copy of the song the piano roll and the tracker draw from.
#pragma GCC optimize("Os")
#include "App.h"
#include <string.h>

namespace roll {

static atm::Score shadow;               // the song again, AHEAD ticks in front
static atm::Osc voices[4];              // what it would play (never sounds)
static uint32_t done;                   // ticks it has run
static Cell ring[RING][4];

uint8_t pitchOf(uint16_t f) {
    if (!f) return 0;
    int lo = 1, hi = 63;
    while (lo < hi) {                   // the first note at or above f
        int mid = (lo + hi) / 2;
        if (atm::noteFreq(mid) < f) lo = mid + 1;
        else hi = mid;
    }
    if (lo > 1 && f - atm::noteFreq(lo - 1) < atm::noteFreq(lo) - f) lo--;
    return (uint8_t)lo;
}

void begin(const uint8_t *song) {
    atm::scoreLoad(shadow, song);
    memset(voices, 0, sizeof voices);
    voices[3].freq = 1;
    memset(ring, 0, sizeof ring);
    done = 0;
}

void update(uint32_t now) {
    while (done < now + AHEAD && !shadow.ended) {
        atm::scoreTick(shadow, voices);
        Cell *c = ring[done % RING];
        for (uint8_t n = 0; n < 4; n++) {
            const atm::Chan &ch = shadow.ch[n];
            uint8_t v = voices[n].vol > 127 ? 127 : voices[n].vol;
            uint8_t p = !v ? 0 : n == 3 ? ch.note : pitchOf(voices[n].freq);
            c[n].pitch = p;
            c[n].vol = (uint8_t)(v | (ch.trig ? 0x80 : 0));
        }
        done++;
    }
}

const Cell &at(uint32_t tick, uint8_t ch) {
    static const Cell none = {0, 0};
    // Only what is in the ring: the RING ticks up to the last one run.
    if (tick >= done || tick + RING < done + 1) return none;
    return ring[tick % RING][ch];
}

}  // namespace roll

namespace jukebox {

uint8_t current = 0xFF;
uint8_t section = 0;
bool autoNext = true;
static uint8_t sinceHit = 255;
static uint8_t lastNoise = 0;
static uint8_t ledFrames = 0;

void start(uint8_t song) {
    current = song;
    section = 0;
    ATM.play(SONGS[song].data);
    ATM.check();
    roll::begin(SONGS[song].data);
    roll::update(0);
}

void stop() {
    ATM.stop();
    current = 0xFF;
    section = 0;
    ATM.led(false);
}

uint32_t now() {
    uint32_t t = ATM.ticks();
    return t ? t - 1 : 0;
}

uint16_t pass() {
    if (current == 0xFF) return 0;
    const SongInfo &s = SONGS[current];
    uint32_t t = now();
    return t < s.intro ? 0 : (uint16_t)((t - s.intro) / s.loop + 1);
}

uint8_t hit() { return sinceHit; }

const char *sectionName() {
    if (current == 0xFF) return "STOPPED";
    const SongInfo &s = SONGS[current];
    if (section && section <= s.sectionCount) return s.sections[section - 1];
    return "INTRO";
}

void update() {
    if (current == 0xFF) return;
    roll::update(now());
    uint8_t c = ATM.check();
    if (c) section = c;
    // The beat: a drum hit is the noise voice jumping up.
    atm::Osc o[4];
    ATM.snapshot(o);
    uint8_t nv = o[3].vol;
    if (sinceHit < 255) sinceHit++;
    if (nv > lastNoise + 8 && !ATM.paused()) {
        sinceHit = 0;
        ledFrames = 3;
    }
    lastNoise = nv;
    if (ledFrames) ledFrames--;
    ATM.led(ledFrames != 0);
    // Two passes of the loop, then the next song.
    if (autoNext && pass() > 2) start((uint8_t)((current + 1) % SONG_COUNT));
}

}  // namespace jukebox

// ---- Small shared things --------------------------------------------------------

char *noteName(char *p, uint8_t pitch) {
    static const char NAMES[] = "C-C#D-D#E-F-F#G-G#A-A#B-";
    if (!pitch) {
        *p++ = '-'; *p++ = '-'; *p++ = '-';
    } else {
        uint8_t n = (uint8_t)(pitch - 1);
        *p++ = NAMES[(n % 12) * 2];
        *p++ = NAMES[(n % 12) * 2 + 1];
        *p++ = (char)('2' + n / 12);
    }
    *p = 0;
    return p;
}

const char *drumName(uint8_t tag) {
    switch (tag) {
    case 1: return "KCK";
    case 3: return "SNR";
    case 4: return "GHO";
    case 13: return "TCK";
    case 25: return "TIC";
    case 38: return "BRS";
    case 42: return "HAT";
    case 46: return "OPN";
    case 49: return "RSR";
    case 51: return "RDE";
    default: return "NSE";
    }
}

void noteIcon(int x, int y, uint8_t c) {
    gfx_fillRect(x, y + 6, 3, 3, c);         // the head
    gfx_vline(x + 2, y, 7, c);               // the stem
    gfx_hline(x + 3, y, 2, c);               // the flag
    gfx_pixel(x + 5, y + 1, c);
    gfx_pixel(x + 5, y + 2, c);
    gfx_pixel(x + 4, y + 3, c);
}

ATM_SFX_TRACK(SFX_MOVE, ATM_VOL(22), ATM_SL_VOL((uint8_t)-5), ATM_NOTE_E6, ATM_DELAY(4));
ATM_SFX_TRACK(SFX_OPEN, ATM_VOL(30), ATM_SL_VOL((uint8_t)-2), ATM_NOTE_C5, ATM_DELAY(2), ATM_NOTE_E5, ATM_DELAY(2),
              ATM_NOTE_G5, ATM_DELAY(2), ATM_NOTE_C6, ATM_DELAY(10));
ATM_SFX_TRACK(SFX_BACK, ATM_VOL(26), ATM_SL_VOL((uint8_t)-2), ATM_NOTE_G5, ATM_DELAY(2), ATM_NOTE_C5, ATM_DELAY(8));
ATM_SFX_TRACK(SFX_MODE, ATM_VOL(26), ATM_SL_VOL((uint8_t)-3), ATM_NOTE_C6, ATM_DELAY(3), ATM_NOTE_G6, ATM_DELAY(6));

void sfx(Sfx s) {
    static const uint8_t *const T[] = {SFX_MOVE, SFX_OPEN, SFX_BACK, SFX_MODE};
    // The pulse voice, the lead's: over a song, the effect takes it for a
    // moment and gives it back.
    ATM.playSfx(T[(uint8_t)s], CH_ZERO);
}
