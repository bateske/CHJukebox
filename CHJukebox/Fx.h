// Sizzle for CHJukebox: the CHGame library's chgame/Sizzle with this app's
// switches (the library's Sizzle.h lists them all), compiled in Fx.cpp.
//
// No particles, as in the other apps: the screen is the music's. What is
// left is the pop-in banner (in the chrome's violet) for a song, a view, a
// mode or the drive changing.
#pragma once
#include <CHGame.h>

#define SIZZLE_CONFIGURED 1
#define SIZZLE_NO_PARTICLES 1
#define SIZZLE_KIND_SPARK 0
#define SIZZLE_KIND_STAR 0
#define SIZZLE_KIND_DUST 0
#define SIZZLE_BURST 0
#define SIZZLE_FLOATS 0
#define SIZZLE_HUES_EXPORT 0
#define SIZZLE_HOLD_BANNER 0
#define SIZZLE_SHAKE 0
#define SIZZLE_STYLES SIZZLE_GREEN
#include <chgame/Sizzle.h>
