// The jukebox's songs. Written by tools/songs/make_songs.py: do not edit by hand.
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
static const uint8_t SONG_COUNT = 5;
