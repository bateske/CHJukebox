// "CIRCUIT BREAKER" - DRUM & BASS, F MINOR. 1217 bytes.
// Written by tools/songs/make_songs.py (jungle.py): do not edit by hand.
// Tempo 46 (172 BPM: 16 ticks a beat, 4 beats a bar).
// 4 bars of intro, then 20 bars that loop (every channel's loop track ends with
// ATM_GOTO(itself), so the loop has no seam). CH0 pulse, CH1 square, CH2 saw, CH3 noise.
//
// The brief: "Mid-90s jungle at 172 BPM in F minor, the liquid kind. The
// noise channel is the star: a chopped breakbeat in the spirit of the Amen,
// with ghost snares between the hits, a kick that thuds, a 32nd-note snare
// roll and a break edit every fourth bar. Under it a 'reese' bass on the saw:
// long, sustained notes with a slow vibrato for that detuned wobble. The
// square plays a dreamy pad of slow three-note arpeggios with tremolo over
// Fm7 - Dbmaj7 - Bbm7 - C7, and the pulse a sparse lead whose every note is
// echoed twice, quieter, a dotted eighth apart: a dub delay made of notes.
// Form: four bars of intro (pad, hats and ghost notes, the lead far away, a
// noise riser into the drop), the drop (eight bars), a half-time switch-up
// where the bass dives, and the second drop with the lead an octave up."
// CH0 pulse: the lead and its echoes. CH1 square: the pad. CH2 saw: the
// reese. CH3 noise: the break.
#pragma once
#include "../atm/ATMcmds.h"

const uint8_t jungle[] = {
    0x1C,  // tracks
    0x00, 0x00,  // track 0: silent (unused channels; never called)
    0x01, 0x00,  // track 1: ch0 intro
    0x67, 0x00,  // track 2: ch0 loop
    0x15, 0x01,  // track 3: ch0 bar 6
    0x2E, 0x01,  // track 4: ch0 bar 7
    0x47, 0x01,  // track 5: ch0 bar 8
    0x60, 0x01,  // track 6: ch0 bar 9
    0x79, 0x01,  // track 7: ch1 intro
    0x8B, 0x01,  // track 8: ch1 loop
    0xB5, 0x01,  // track 9: ch1 bar 2
    0xBB, 0x01,  // track 10: ch1 bar 3
    0xC1, 0x01,  // track 11: ch1 bar 4
    0xC7, 0x01,  // track 12: ch1 bar 5
    0xCD, 0x01,  // track 13: ch2 intro
    0xD4, 0x01,  // track 14: ch2 loop
    0x23, 0x02,  // track 15: ch2 bar 6
    0x2C, 0x02,  // track 16: ch2 bar 7
    0x35, 0x02,  // track 17: ch2 bar 8
    0x3E, 0x02,  // track 18: ch2 bar 9
    0x47, 0x02,  // track 19: ch2 bar 20
    0x50, 0x02,  // track 20: ch3 intro
    0x90, 0x02,  // track 21: ch3 loop
    0x0A, 0x03,  // track 22: ch3 bar 1
    0x3B, 0x03,  // track 23: ch3 bar 5
    0x7E, 0x03,  // track 24: ch3 bar 6
    0xBD, 0x03,  // track 25: ch3 bar 8
    0x13, 0x04,  // track 26: ch3 bar 13
    0x45, 0x04,  // track 27: ch3 bar 14
    0x01,  // CH0 entry
    0x07,  // CH1 entry
    0x0D,  // CH2 entry
    0x14,  // CH3 entry
    // track 0: silent (unused channels; never called)
    ATM_STOP_CHAN,
    // track 1: ch0 intro
    ATM_SET_TEMPO(46), ATM_SL_VOL((uint8_t)-1), ATM_VOL(14), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(6),
    ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(2), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(14), ATM_NOTE_D6_,
    ATM_DELAY(12), ATM_VOL(6), ATM_NOTE_D6_, ATM_DELAY(12), ATM_VOL(2), ATM_NOTE_D6_, ATM_DELAY(4),
    ATM_VOL(14), ATM_NOTE_F6, ATM_DELAY(12), ATM_VOL(6), ATM_NOTE_F6, ATM_DELAY(12), ATM_VOL(14),
    ATM_NOTE_D6_, ATM_DELAY(12), ATM_VOL(14), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(6), ATM_NOTE_C6,
    ATM_DELAY(12), ATM_VOL(2), ATM_NOTE_C6, ATM_DELAY(4), ATM_VOL(14), ATM_NOTE_G5_, ATM_DELAY(12),
    ATM_VOL(6), ATM_NOTE_G5_, ATM_DELAY(12), ATM_VOL(2), ATM_NOTE_G5_, ATM_DELAY(12), ATM_VOL(14),
    ATM_NOTE_A5_, ATM_DELAY(12), ATM_VOL(6), ATM_NOTE_A5_, ATM_DELAY(12), ATM_VOL(2), ATM_NOTE_A5_,
    ATM_DELAY(4), ATM_VOL(14), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(6), ATM_NOTE_C6, ATM_DELAY(12),
    ATM_VOL(2), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(14), ATM_NOTE_G5, ATM_DELAY(12), ATM_VOL(6),
    ATM_NOTE_G5, ATM_DELAY(12), ATM_VOL(2), ATM_NOTE_G5, ATM_DELAY(4), ATM_GOTO(2),
    // track 2: ch0 loop
    ATM_SET_TEMPO(46), ATM_VOL(30), ATM_NOTE_C6, ATM_CUE(1), ATM_DELAY(12), ATM_VOL(13),
    ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(5), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(30), ATM_NOTE_D6_,
    ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_D6_, ATM_DELAY(12), ATM_VOL(5), ATM_NOTE_D6_, ATM_DELAY(4),
    ATM_GOTO(3), ATM_GOTO(4), ATM_GOTO(5), ATM_GOTO(6), ATM_GOTO(3), ATM_GOTO(4), ATM_GOTO(5), 0x00,
    ATM_CUE(2), ATM_DELAY(64), ATM_DELAY(64), ATM_DELAY(32), ATM_VOL(30), ATM_NOTE_F5,
    ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_F5, ATM_DELAY(4), ATM_VOL(30), ATM_NOTE_G5_, ATM_DELAY(12),
    ATM_VOL(13), ATM_NOTE_G5_, ATM_DELAY(4), ATM_VOL(30), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(13),
    ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(5), ATM_NOTE_C6, ATM_DELAY(8), ATM_VOL(30), ATM_NOTE_A5_,
    ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_A5_, ATM_DELAY(12), ATM_VOL(5), ATM_NOTE_A5_, ATM_DELAY(8),
    ATM_VOL(30), ATM_NOTE_F6, ATM_CUE(3), ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_F6, ATM_DELAY(12),
    ATM_VOL(5), ATM_NOTE_F6, ATM_DELAY(12), ATM_VOL(30), ATM_NOTE_G6_, ATM_DELAY(12), ATM_VOL(13),
    ATM_NOTE_G6_, ATM_DELAY(12), ATM_VOL(5), ATM_NOTE_G6_, ATM_DELAY(4), ATM_VOL(30), ATM_NOTE_A6_,
    ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_A6_, ATM_DELAY(12), ATM_VOL(30), ATM_NOTE_G6_,
    ATM_DELAY(12), ATM_VOL(30), ATM_NOTE_F6, ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_F6, ATM_DELAY(12),
    ATM_VOL(5), ATM_NOTE_F6, ATM_DELAY(4), ATM_GOTO(6), ATM_VOL(30), ATM_NOTE_F6, ATM_DELAY(12),
    ATM_VOL(13), ATM_NOTE_F6, ATM_DELAY(12), ATM_VOL(30), ATM_NOTE_G6, ATM_DELAY(12), ATM_VOL(30),
    ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(5), ATM_NOTE_C6,
    ATM_DELAY(4), ATM_GOTO(6), ATM_GOTO(3), ATM_GOTO(4), ATM_GOTO(5), ATM_GOTO(2),
    // track 3: ch0 bar 6
    ATM_VOL(30), ATM_NOTE_F6, ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_F6, ATM_DELAY(12), ATM_VOL(30),
    ATM_NOTE_D6_, ATM_DELAY(12), ATM_VOL(30), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_C6,
    ATM_DELAY(12), ATM_VOL(5), ATM_NOTE_C6, ATM_DELAY(4), ATM_RETURN,
    // track 4: ch0 bar 7
    ATM_VOL(30), ATM_NOTE_G5_, ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_G5_, ATM_DELAY(12), ATM_VOL(5),
    ATM_NOTE_G5_, ATM_DELAY(12), ATM_VOL(30), ATM_NOTE_A5_, ATM_DELAY(12), ATM_VOL(13),
    ATM_NOTE_A5_, ATM_DELAY(12), ATM_VOL(5), ATM_NOTE_A5_, ATM_DELAY(4), ATM_RETURN,
    // track 5: ch0 bar 8
    ATM_VOL(30), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(5),
    ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(30), ATM_NOTE_G5, ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_G5,
    ATM_DELAY(12), ATM_VOL(5), ATM_NOTE_G5, ATM_DELAY(4), ATM_RETURN,
    // track 6: ch0 bar 9
    ATM_VOL(30), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(5),
    ATM_NOTE_C6, ATM_DELAY(12), ATM_VOL(30), ATM_NOTE_D6_, ATM_DELAY(12), ATM_VOL(13), ATM_NOTE_D6_,
    ATM_DELAY(12), ATM_VOL(5), ATM_NOTE_D6_, ATM_DELAY(4), ATM_RETURN,
    // track 7: ch1 intro
    ATM_VOL(16), ATM_TREM(1, 7), ATM_ARP(0x43, 0x23), ATM_NOTE_G4_, ATM_DELAY(64), ATM_GOTO(9),
    ATM_GOTO(10), ATM_GOTO(11), ATM_GOTO(8),
    // track 8: ch1 loop
    ATM_GOTO(12), ATM_GOTO(9), ATM_GOTO(10), ATM_GOTO(11), ATM_GOTO(12), ATM_GOTO(9), ATM_GOTO(10),
    ATM_GOTO(11), ATM_GOTO(12), ATM_GOTO(9), ATM_GOTO(10), ATM_GOTO(11), ATM_GOTO(12), ATM_GOTO(9),
    ATM_GOTO(10), ATM_GOTO(11), ATM_GOTO(12), ATM_GOTO(9), ATM_GOTO(10), ATM_GOTO(11), ATM_GOTO(8),
    // track 9: ch1 bar 2
    ATM_ARP(0x34, 0x23), ATM_NOTE_F4, ATM_DELAY(64), ATM_RETURN,
    // track 10: ch1 bar 3
    ATM_ARP(0x43, 0x23), ATM_NOTE_C5_, ATM_DELAY(64), ATM_RETURN,
    // track 11: ch1 bar 4
    ATM_ARP(0x33, 0x23), ATM_NOTE_E4, ATM_DELAY(64), ATM_RETURN,
    // track 12: ch1 bar 5
    ATM_ARP(0x43, 0x23), ATM_NOTE_G4_, ATM_DELAY(64), ATM_RETURN,
    // track 13: ch2 intro
    0x00, ATM_DELAY(64), ATM_DELAY(64), ATM_DELAY(64), ATM_DELAY(64), ATM_GOTO(14),
    // track 14: ch2 loop
    ATM_VOL(52), ATM_SL_VOL(0), ATM_VIB(1, 7), ATM_NOTE_F3, ATM_DELAY(28), ATM_NOTE_F3,
    ATM_DELAY(12), ATM_NOTE_C4, ATM_DELAY(8), ATM_NOTE_F3, ATM_DELAY(16), ATM_GOTO(15),
    ATM_GOTO(16), ATM_GOTO(17), ATM_GOTO(18), ATM_GOTO(15), ATM_GOTO(16), ATM_GOTO(17), ATM_NOTE_F4,
    ATM_DELAY(32), ATM_SL_FRQ((uint8_t)-10), ATM_DELAY(24), ATM_SL_FRQ_OFF, 0x00, ATM_DELAY(8),
    ATM_NOTE_C4_, ATM_DELAY(32), ATM_SL_FRQ((uint8_t)-10), ATM_DELAY(24), ATM_SL_FRQ_OFF, 0x00,
    ATM_DELAY(8), ATM_NOTE_A3_, ATM_DELAY(32), ATM_SL_FRQ((uint8_t)-10), ATM_DELAY(24),
    ATM_SL_FRQ_OFF, 0x00, ATM_DELAY(8), ATM_NOTE_C4, ATM_DELAY(32), ATM_SL_FRQ((uint8_t)-10),
    ATM_DELAY(24), ATM_SL_FRQ_OFF, 0x00, ATM_DELAY(8), ATM_GOTO(18), ATM_GOTO(15), ATM_GOTO(16),
    ATM_GOTO(19), ATM_GOTO(18), ATM_GOTO(15), ATM_GOTO(16), ATM_GOTO(19), ATM_GOTO(14),
    // track 15: ch2 bar 6
    ATM_NOTE_C3_, ATM_DELAY(28), ATM_NOTE_C3_, ATM_DELAY(12), ATM_NOTE_C4_, ATM_DELAY(8),
    ATM_NOTE_C3_, ATM_DELAY(16), ATM_RETURN,
    // track 16: ch2 bar 7
    ATM_NOTE_A2_, ATM_DELAY(28), ATM_NOTE_A2_, ATM_DELAY(12), ATM_NOTE_F3, ATM_DELAY(8),
    ATM_NOTE_A2_, ATM_DELAY(16), ATM_RETURN,
    // track 17: ch2 bar 8
    ATM_NOTE_C3, ATM_DELAY(28), ATM_NOTE_C3, ATM_DELAY(12), ATM_NOTE_C4, ATM_DELAY(8), ATM_NOTE_C3,
    ATM_DELAY(16), ATM_RETURN,
    // track 18: ch2 bar 9
    ATM_NOTE_F3, ATM_DELAY(28), ATM_NOTE_F3, ATM_DELAY(12), ATM_NOTE_C4, ATM_DELAY(8), ATM_NOTE_F3,
    ATM_DELAY(16), ATM_RETURN,
    // track 19: ch2 bar 20
    ATM_NOTE_C3, ATM_DELAY(16), ATM_NOTE_D3_, ATM_DELAY(16), ATM_NOTE_G3, ATM_DELAY(16),
    ATM_NOTE_A3_, ATM_DELAY(16), ATM_RETURN,
    // track 20: ch3 intro
    ATM_GOTO(22), ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8), ATM_NOTE_F5,
    ATM_VOL(10), ATM_DELAY(8), ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_, ATM_VOL(16), ATM_DELAY(8),
    ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8), ATM_NOTE_F5, ATM_VOL(10),
    ATM_DELAY(8), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(4), ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_,
    ATM_VOL(16), ATM_DELAY(4), ATM_NOTE_D2_, ATM_VOL(16), ATM_DELAY(8), ATM_SL_VOL((uint8_t)-5),
    ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8), ATM_GOTO(22), ATM_SL_VOL(1), ATM_NOTE_C6, ATM_VOL(2),
    ATM_DELAY(62), ATM_SL_VOL((uint8_t)-6), ATM_NOTE_D2, ATM_VOL(30), ATM_DELAY(2), ATM_GOTO(21),
    // track 21: ch3 loop
    ATM_GOTO(23), ATM_GOTO(24), ATM_GOTO(23), ATM_GOTO(25), ATM_GOTO(23), ATM_GOTO(24),
    ATM_GOTO(23), ATM_GOTO(25), ATM_GOTO(26), ATM_GOTO(27), ATM_GOTO(26), ATM_GOTO(27),
    ATM_GOTO(23), ATM_GOTO(24), ATM_GOTO(23), ATM_GOTO(25), ATM_GOTO(23), ATM_GOTO(24),
    ATM_GOTO(23), ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-12), ATM_NOTE_C2, ATM_VOL(54), ATM_DELAY(8),
    ATM_NOISE_OFF, ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8),
    ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2, ATM_VOL(48), ATM_DELAY(8), ATM_SL_VOL((uint8_t)-5),
    ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(4), ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_, ATM_VOL(16),
    ATM_DELAY(4), ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-12), ATM_NOTE_C2, ATM_VOL(54), ATM_DELAY(8),
    ATM_NOISE_OFF, ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2, ATM_VOL(48), ATM_DELAY(8),
    ATM_SL_VOL((uint8_t)-6), ATM_NOTE_D2, ATM_VOL(14), ATM_DELAY(2), ATM_NOTE_D2, ATM_VOL(20),
    ATM_DELAY(2), ATM_NOTE_D2, ATM_VOL(26), ATM_DELAY(2), ATM_NOTE_D2, ATM_VOL(32), ATM_DELAY(2),
    ATM_NOTE_D2, ATM_VOL(38), ATM_DELAY(2), ATM_NOTE_D2, ATM_VOL(44), ATM_DELAY(2), ATM_NOTE_D2,
    ATM_VOL(50), ATM_DELAY(2), ATM_NOTE_D2, ATM_VOL(56), ATM_DELAY(2), ATM_GOTO(21),
    // track 22: ch3 bar 1
    ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8), ATM_NOTE_F5, ATM_VOL(10),
    ATM_DELAY(8), ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_, ATM_VOL(16), ATM_DELAY(8),
    ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8), ATM_NOTE_F5, ATM_VOL(10),
    ATM_DELAY(8), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8), ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_,
    ATM_VOL(16), ATM_DELAY(8), ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(4),
    ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_, ATM_VOL(16), ATM_DELAY(4), ATM_RETURN,
    // track 23: ch3 bar 5
    ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-12), ATM_NOTE_C2, ATM_VOL(54), ATM_DELAY(8),
    ATM_NOISE_OFF, ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8),
    ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2, ATM_VOL(48), ATM_DELAY(8), ATM_SL_VOL((uint8_t)-5),
    ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(4), ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_, ATM_VOL(16),
    ATM_DELAY(8), ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(4), ATM_NOISE(0x14),
    ATM_SL_VOL((uint8_t)-12), ATM_NOTE_C2, ATM_VOL(54), ATM_DELAY(8), ATM_NOISE_OFF,
    ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2, ATM_VOL(48), ATM_DELAY(8), ATM_SL_VOL((uint8_t)-5),
    ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(4), ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_, ATM_VOL(16),
    ATM_DELAY(4), ATM_RETURN,
    // track 24: ch3 bar 6
    ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-12), ATM_NOTE_C2, ATM_VOL(54), ATM_DELAY(8),
    ATM_NOISE_OFF, ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8),
    ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2, ATM_VOL(48), ATM_DELAY(8), ATM_SL_VOL((uint8_t)-5),
    ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(4), ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_, ATM_VOL(16),
    ATM_DELAY(8), ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(4), ATM_NOISE(0x14),
    ATM_SL_VOL((uint8_t)-12), ATM_NOTE_C2, ATM_VOL(54), ATM_DELAY(4), ATM_NOTE_C2, ATM_VOL(54),
    ATM_DELAY(8), ATM_NOISE_OFF, ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_, ATM_VOL(16), ATM_DELAY(4),
    ATM_NOTE_D2, ATM_VOL(48), ATM_DELAY(8), ATM_RETURN,
    // track 25: ch3 bar 8
    ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-12), ATM_NOTE_C2, ATM_VOL(54), ATM_DELAY(8),
    ATM_NOISE_OFF, ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8),
    ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2, ATM_VOL(48), ATM_DELAY(8), ATM_SL_VOL((uint8_t)-5),
    ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(4), ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_, ATM_VOL(16),
    ATM_DELAY(4), ATM_SL_VOL((uint8_t)-6), ATM_NOTE_D2, ATM_VOL(18), ATM_DELAY(2), ATM_NOTE_D2,
    ATM_VOL(22), ATM_DELAY(2), ATM_NOTE_D2, ATM_VOL(26), ATM_DELAY(2), ATM_NOTE_D2, ATM_VOL(30),
    ATM_DELAY(2), ATM_NOTE_D2, ATM_VOL(34), ATM_DELAY(2), ATM_NOTE_D2, ATM_VOL(38), ATM_DELAY(2),
    ATM_NOTE_D2, ATM_VOL(42), ATM_DELAY(2), ATM_NOTE_D2, ATM_VOL(46), ATM_DELAY(2),
    ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2, ATM_VOL(48), ATM_DELAY(8), ATM_NOTE_D2, ATM_VOL(48),
    ATM_DELAY(4), ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-12), ATM_NOTE_C2, ATM_VOL(54), ATM_DELAY(4),
    ATM_RETURN,
    // track 26: ch3 bar 13
    ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-12), ATM_NOTE_C2, ATM_VOL(54), ATM_DELAY(8),
    ATM_NOISE_OFF, ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8), ATM_NOTE_F5,
    ATM_VOL(10), ATM_DELAY(8), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8), ATM_SL_VOL((uint8_t)-4),
    ATM_NOTE_D2, ATM_VOL(48), ATM_DELAY(8), ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10),
    ATM_DELAY(8), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(4),
    ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_, ATM_VOL(16), ATM_DELAY(4), ATM_RETURN,
    // track 27: ch3 bar 14
    ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-12), ATM_NOTE_C2, ATM_VOL(54), ATM_DELAY(8),
    ATM_NOISE_OFF, ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8), ATM_NOTE_F5,
    ATM_VOL(10), ATM_DELAY(8), ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-12), ATM_NOTE_C2, ATM_VOL(54),
    ATM_DELAY(8), ATM_NOISE_OFF, ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2, ATM_VOL(48), ATM_DELAY(8),
    ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(8), ATM_NOTE_F5, ATM_VOL(10),
    ATM_DELAY(4), ATM_SL_VOL((uint8_t)-4), ATM_NOTE_D2_, ATM_VOL(16), ATM_DELAY(4),
    ATM_SL_VOL((uint8_t)-5), ATM_NOTE_F5, ATM_VOL(10), ATM_DELAY(4), ATM_SL_VOL((uint8_t)-4),
    ATM_NOTE_D2, ATM_VOL(48), ATM_DELAY(4), ATM_RETURN,
};
