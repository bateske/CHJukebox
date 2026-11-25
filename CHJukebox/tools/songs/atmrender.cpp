// atmrender: CHJukebox's port of ATMlib (src/atm) on the PC. Plays a song's
// bytes through the same playroutine and mixer the board runs, and writes
//   - a WAV of the mix (16-bit mono, 15,625 Hz: the board's own samples), and
//   - a timeline: one line per tick (the tick, the tempo, then per channel
//     note,freq,vol,trig,depth,chanVol,chanFreq: the voice as mixed, then the
//     channel's own state; then the cue and the active mask), for make_songs.py's loop and range checks.
//
//   atmrender SONG.bin SECONDS OUT.wav [TIMELINE.txt] [DRIVE]
//
// Built and run by make_songs.py (zig c++, -DATM_HOST).
#include "../../src/atm/ATMlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

static void le16(FILE *f, uint32_t v) { fputc(v & 255, f); fputc((v >> 8) & 255, f); }
static void le32(FILE *f, uint32_t v) { le16(f, v & 0xFFFF); le16(f, v >> 16); }

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "usage: atmrender SONG.bin SECONDS OUT.wav [TIMELINE.txt] [DRIVE]\n");
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror(argv[1]); return 2; }
    std::vector<uint8_t> song;
    int c;
    while ((c = fgetc(f)) != EOF) song.push_back((uint8_t)c);
    fclose(f);
    double seconds = atof(argv[2]);
    FILE *tl = argc > 4 && strcmp(argv[4], "-") ? fopen(argv[4], "w") : nullptr;

    ATMsynth ATM;
    if (argc > 5) ATM.setDrive((uint8_t)atoi(argv[5]));
    ATM.play(song.data());
    uint32_t n = (uint32_t)(seconds * atm::RATE);
    std::vector<int16_t> pcm(n);
    uint32_t lastTick = 0, clipped = 0, peak = 0;
    uint64_t energy = 0;
    for (uint32_t i = 0; i < n; i++) {
        uint8_t v = ATMsynth::render();
        if (v == 0 || v == 255) clipped++;
        pcm[i] = (int16_t)((v - 128) * 256);
        energy += (uint64_t)((v - 128) * (v - 128));
        uint32_t t = ATM.ticks();
        if (tl && t != lastTick) {
            // The state after tick t-1 ran: what sounds during it.
            const atm::Score &s = ATM.score();
            atm::Osc o[4];
            ATM.snapshot(o);
            fprintf(tl, "%u %u", t - 1, s.tickRate);
            for (int k = 0; k < 4; k++) {
                const atm::Chan &ch = s.ch[k];
                fprintf(tl, " %d,%u,%u,%u,%u,%u,%u", ch.note, k == 3 ? 0u : (unsigned)o[k].freq, (unsigned)o[k].vol, ch.trig,
                        (unsigned)ch.stackIndex, (unsigned)ch.vol, (unsigned)ch.freq);
            }
            fprintf(tl, " %u %u\n", s.cue, s.active);
            lastTick = t;
        }
        uint16_t p = ATM.peak();
        if (p > peak) peak = p;
        if (!ATM.playing() && i > 0) { n = i + 1; break; }
    }
    if (tl) fclose(tl);

    FILE *w = fopen(argv[3], "wb");
    if (!w) { perror(argv[3]); return 2; }
    fwrite("RIFF", 1, 4, w); le32(w, 36 + n * 2); fwrite("WAVEfmt ", 1, 8, w);
    le32(w, 16); le16(w, 1); le16(w, 1); le32(w, atm::RATE); le32(w, atm::RATE * 2); le16(w, 2); le16(w, 16);
    fwrite("data", 1, 4, w); le32(w, n * 2);
    fwrite(pcm.data(), 2, n, w);
    fclose(w);
    printf("samples %u ticks %u peak %u clipped %u rms %.1f ended %d\n", n, ATM.ticks(), peak, clipped,
           n ? __builtin_sqrt((double)energy / n) : 0.0, ATM.playing() ? 0 : 1);
    return 0;
}
