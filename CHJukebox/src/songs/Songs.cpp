// The jukebox's songs. Written by tools/songs/make_songs.py: do not edit by hand.
#include "Songs.h"
#include "invention.h"
#include "jig.h"
#include "funk.h"
#include "jungle.h"
#include "nocturne.h"

static const char *const inventionSections[] = {"EXPOSITION", "EPISODE", "F MAJOR", "EPISODE II", "RETURN", "PEDAL", "CADENCE"};
static const char *const jigSections[] = {"A PART", "A AGAIN", "B PART", "B AGAIN"};
static const char *const funkSections[] = {"GROOVE", "HOOK", "BREAKDOWN", "HOOK"};
static const char *const jungleSections[] = {"DROP", "SWITCH UP", "DROP II"};
static const char *const nocturneSections[] = {"THE TUNE", "AGAIN, HIGHER"};

const SongInfo SONGS[] = {
    {invention, sizeof invention, "CLOCKWORK INVENTION", "BAROQUE", "D MINOR",
     "TWO-PART INVENTION FOR A CLOCKWORK HARPSICHORD", 112, 16, 4, 7, 4, 10, 63, 0, 1024, inventionSections},
    {jig, sizeof jig, "THE BRASS KETTLE", "CELTIC JIG", "D MAJOR",
     "AN IRISH JIG IN 6/8 WITH DRONES AND BODHRAN", 117, 18, 2, 4, 6, 12, 56, 72, 1152, jigSections},
    {funk, sizeof funk, "VELVET STRUT", "DISCO FUNK", "E DORIAN",
     "SLAP BASS, CHICKEN-SCRATCH GUITAR, FOUR ON THE FLOOR", 109, 16, 4, 4, 4, 10, 55, 128, 1536, funkSections},
    {jungle, sizeof jungle, "CIRCUIT BREAKER", "DRUM & BASS", "F MINOR",
     "CHOPPED BREAKBEATS, REESE BASS, DUB ECHOES", 172, 16, 4, 3, 4, 11, 59, 256, 1280, jungleSections},
    {nocturne, sizeof nocturne, "RAIN ON GLASS", "LO-FI JAZZ", "DB MAJOR",
     "A LATE-NIGHT BALLAD, EXTENDED CHORDS, BRUSHES", 75, 24, 4, 2, 8, 9, 59, 192, 1536, nocturneSections},
};
