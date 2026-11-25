// The voices' waveforms, drawn with the mixer's own arithmetic (ATMlib.cpp's
// sample()) from a snapshot of the oscillators: the picture is the sound.
// A voice's scope shows two of its cycles whatever its pitch, so it stands
// still; the mix's is time, 4 samples a pixel, started where the lowest
// voice begins a cycle.
#pragma GCC optimize("O2")
#include "App.h"

namespace scope {

static uint16_t lfsrStep(uint16_t x) {
    x = (uint16_t)(x << 1);
    if (x & 0x8000) x ^= 1;
    if (x & 0x4000) x ^= 1;
    return x;
}

// What a voice adds to the mix at a phase (the noise voice: at its LFSR).
static int level(uint8_t ch, uint32_t phase, int vol, uint16_t lfsr) {
    switch (ch) {
    case 0: return (phase >> 24) >= 0xC0 ? -vol : vol;
    case 1: return (phase & 0x80000000u) ? -vol : vol;
    case 2: return ((int)(int8_t)((phase >> 24) - 128) * vol) >> 8;
    default: return (lfsr & 0x8000) ? -vol : vol;
    }
}

// A trace: a column from the last point to this one, so steep edges join up.
static void trace(int x, const int8_t *ys, int n, int mid, uint8_t c) {
    int prev = mid + ys[0];
    for (int i = 0; i < n; i++) {
        int y = mid + ys[i];
        if (y == prev) gfx_pixel(x + i, y, c);
        else if (y > prev) gfx_vline(x + i, prev + 1, y - prev, c);
        else gfx_vline(x + i, y, prev - y, c);
        prev = y;
    }
}

void voice(const atm::Osc &o, uint8_t ch, int x, int y, int w, int h, uint8_t c) {
    int mid = y + h / 2, amp = h / 2;
    int8_t ys[32];
    if (w > 32) w = 32;
    if (!o.vol || (ch < 3 && !o.step)) {
        gfx_hline(x, mid, w, agent::GRID);
        return;
    }
    // Full volume fills the box; the saw (mixed at half) is shown at full.
    int vol = o.vol > 63 ? 63 : o.vol;
    uint32_t dp = (uint32_t)(0x200000000ULL / (uint32_t)w);
    uint16_t lfsr = (uint16_t)(o.freq ^ (uint16_t)(chgame.frameCount * 40503u));
    for (int i = 0; i < w; i++) {
        int v;
        if (ch == 3) {
            lfsr = lfsrStep(lfsrStep(lfsrStep(lfsr)));
            v = level(3, 0, vol, lfsr);
        } else {
            v = level(ch, (uint32_t)i * dp, vol, 0);
            if (ch == 2) v *= 2;
        }
        ys[i] = (int8_t)(-v * amp / 64);
    }
    trace(x, ys, w, mid, c);
}

void mix(const atm::Osc *o, uint8_t muted, int x, int y, int w, int h, uint8_t c, uint8_t trail, int8_t *last) {
    static const uint32_t SPP = 4;                  // samples a pixel
    int8_t ys[128];
    if (w > 128) w = 128;
    // Start where the lowest sounding tonal voice starts a cycle.
    int trig = -1;
    for (int ch = 0; ch < 3; ch++)
        if (o[ch].vol && o[ch].step && !(muted & (1 << ch)) && (trig < 0 || o[ch].step < o[trig].step)) trig = ch;
    uint32_t ahead = trig >= 0 ? (0u - o[trig].phase) / o[trig].step : 0;
    uint16_t lfsr = o[3].freq;
    int amp = h / 2, drive = ATM.drive();
    for (int i = 0; i < w; i++) {
        uint32_t s = ahead + (uint32_t)i * SPP;
        int sum = 0;
        for (int ch = 0; ch < 3; ch++)
            if (o[ch].vol) sum += level((uint8_t)ch, o[ch].phase + s * o[ch].step, o[ch].vol, 0);
        for (uint32_t k = 0; k < SPP; k++) lfsr = lfsrStep(lfsr);
        sum += level(3, 0, o[3].vol, lfsr);
        int v = (sum * drive) >> 4;                 // as the PWM gets it: driven, then clipped
        if (v > 127) v = 127;
        if (v < -128) v = -128;
        ys[i] = (int8_t)(-v * amp / 128);
    }
    int mid = y + h / 2;
    if (last) {
        trace(x, last, w, mid, trail);
        for (int i = 0; i < w; i++) last[i] = ys[i];
    }
    trace(x, ys, w, mid, c);
}

}  // namespace scope
