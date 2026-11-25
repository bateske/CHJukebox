// CHJukebox build switches.
//
// Built like the games: any CHGame board (Rev0 and Rev1 from board package
// 0.4.0; the synth finds its timer in the variant), Peripherals "Game" (the
// synth defines the TIM1-TIM3 interrupt handlers and the SysTick hook
// itself; "Full" compiles the core's HardwareTimer, whose handlers would
// clash), Optimize "Smallest + LTO", USB "Upload only" for release.
#pragma once

#define JUKE_VERSION     "1.0"

// The CHGame library's switches: CHGAME_DEBUG (the serial debug protocol;
// always on in the simulator, on the board only in `chgame build --debug`).
#include <chgame/Config.h>

#define JUKE_FPS         60
