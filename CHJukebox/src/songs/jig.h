// "THE BRASS KETTLE" - CELTIC JIG, D MAJOR. 785 bytes.
// Written by tools/songs/make_songs.py (jig.py): do not edit by hand.
// Tempo 35 (117 BPM: 18 ticks a beat, 2 beats a bar).
// 2 bars of intro, then 32 bars that loop (every channel's loop track ends with
// ATM_GOTO(itself), so the loop has no seam). CH0 pulse, CH1 square, CH2 saw, CH3 noise.
//
// The brief: "A rollicking Irish double jig in 6/8 at 117 dotted-quarter
// beats a minute, in D major with a mixolydian turn (C natural) in the high
// part. A fiddle tune of two 8-bar parts played AABB, the way a session plays
// it: the lilt of a jig is in its 'D-D-D' figures and its runs; the second
// time through each part the fiddler ornaments it, with cuts (a flick of a
// higher note before a repeated one), a roll on the long notes and slides up
// into the high notes. Under it a drone of open fifths like uilleann pipes,
// swelling slowly; a bouzouki-style bass that bounces root and fifth on the
// beat; and a bodhran: a deep stroke on each beat, two lighter ones after it,
// and a rolled fill at the end of every part. Two bars of drone and drum to
// count it in."
// CH0 pulse: the fiddle. CH1 square: the drone. CH2 saw: the bass.
// CH3 noise: the bodhran.
#pragma once
#include "../atm/ATMcmds.h"

const uint8_t jig[] = {
    0x1A,  // tracks
    0x00, 0x00,  // track 0: silent (unused channels; never called)
    0x01, 0x00,  // track 1: ch0 intro
    0x08, 0x00,  // track 2: ch0 loop
    0x47, 0x01,  // track 3: ch0 bar 4
    0x54, 0x01,  // track 4: ch0 bar 8
    0x61, 0x01,  // track 5: ch0 bar 9
    0x6E, 0x01,  // track 6: ch0 bar 22
    0x7B, 0x01,  // track 7: ch0 bar 24
    0x88, 0x01,  // track 8: ch0 bar 25
    0x95, 0x01,  // track 9: ch1 intro
    0xA3, 0x01,  // track 10: ch1 loop
    0xA8, 0x01,  // track 11: ch1 bar 2
    0xAB, 0x01,  // track 12: ch2 intro
    0xB0, 0x01,  // track 13: ch2 loop
    0x00, 0x02,  // track 14: ch2 bar 4
    0x0D, 0x02,  // track 15: ch2 bar 5
    0x1A, 0x02,  // track 16: ch2 bar 6
    0x27, 0x02,  // track 17: ch2 bar 8
    0x34, 0x02,  // track 18: ch2 bar 9
    0x41, 0x02,  // track 19: ch2 bar 10
    0x4E, 0x02,  // track 20: ch2 bar 20
    0x5B, 0x02,  // track 21: ch2 bar 21
    0x68, 0x02,  // track 22: ch3 intro
    0x6D, 0x02,  // track 23: ch3 loop
    0x83, 0x02,  // track 24: ch3 bar 1
    0xAA, 0x02,  // track 25: ch3 bar 10
    0x01,  // CH0 entry
    0x09,  // CH1 entry
    0x0C,  // CH2 entry
    0x16,  // CH3 entry
    // track 0: silent (unused channels; never called)
    ATM_STOP_CHAN,
    // track 1: ch0 intro
    ATM_SET_TEMPO(35), 0x00, ATM_DELAY(36), ATM_DELAY(36), ATM_GOTO(2),
    // track 2: ch0 loop
    ATM_SET_TEMPO(35), ATM_VOL(36), ATM_SL_VOL((uint8_t)-1), ATM_VIB(3, 3), ATM_NOTE_D5, ATM_CUE(1),
    ATM_DELAY(6), ATM_NOTE_E5, ATM_DELAY(6), ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6),
    ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(6), ATM_GOTO(3), ATM_NOTE_A4, ATM_DELAY(6),
    ATM_NOTE_D5, ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(6), ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_D5,
    ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(6), ATM_NOTE_E5, ATM_DELAY(6), ATM_NOTE_F5_, ATM_DELAY(6),
    ATM_NOTE_G5, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(12), ATM_NOTE_G5, ATM_DELAY(6), ATM_NOTE_F5_,
    ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_B5, ATM_DELAY(6),
    ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_GOTO(4), ATM_GOTO(5), ATM_NOTE_E5,
    ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(6), ATM_NOTE_C5_, ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(12),
    ATM_NOTE_A4, ATM_DELAY(6), ATM_NOTE_D5, ATM_CUE(2), ATM_DELAY(6), ATM_NOTE_E5, ATM_DELAY(6),
    ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_D5,
    ATM_DELAY(6), ATM_GOTO(3), ATM_NOTE_A4, ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(6), ATM_NOTE_E5,
    ATM_DELAY(1), ATM_NOTE_D5, ATM_DELAY(5), ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(6),
    ATM_NOTE_E5, ATM_DELAY(1), ATM_NOTE_D5, ATM_DELAY(5), ATM_NOTE_E5, ATM_DELAY(6), ATM_NOTE_F5_,
    ATM_DELAY(6), ATM_NOTE_G5, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(4), ATM_NOTE_B5, ATM_DELAY(1),
    ATM_NOTE_A5, ATM_DELAY(3), ATM_NOTE_G5, ATM_DELAY(1), ATM_NOTE_A5, ATM_DELAY(3), ATM_NOTE_G5,
    ATM_DELAY(6), ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_B5, ATM_DELAY(1),
    ATM_NOTE_A5, ATM_DELAY(5), ATM_NOTE_B5, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_B5,
    ATM_DELAY(1), ATM_NOTE_A5, ATM_DELAY(5), ATM_GOTO(4), ATM_GOTO(5), ATM_NOTE_E5, ATM_DELAY(6),
    ATM_NOTE_D5, ATM_DELAY(6), ATM_NOTE_C5_, ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(4), ATM_NOTE_E5,
    ATM_DELAY(1), ATM_NOTE_D5, ATM_DELAY(3), ATM_NOTE_C5_, ATM_DELAY(1), ATM_NOTE_D5, ATM_DELAY(3),
    ATM_NOTE_A4, ATM_DELAY(6), ATM_NOTE_D6, ATM_CUE(3), ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6),
    ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_D6, ATM_DELAY(6), ATM_NOTE_E6,
    ATM_DELAY(6), ATM_NOTE_F6_, ATM_DELAY(6), ATM_NOTE_E6, ATM_DELAY(6), ATM_NOTE_D6, ATM_DELAY(6),
    ATM_NOTE_C6, ATM_DELAY(12), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_B5, ATM_DELAY(6), ATM_NOTE_G5,
    ATM_DELAY(6), ATM_NOTE_G5, ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(6), ATM_NOTE_G5, ATM_DELAY(6),
    ATM_NOTE_G5, ATM_DELAY(6), ATM_GOTO(6), ATM_NOTE_G6, ATM_DELAY(6), ATM_NOTE_F6_, ATM_DELAY(6),
    ATM_NOTE_E6, ATM_DELAY(6), ATM_NOTE_F6_, ATM_DELAY(6), ATM_NOTE_D6, ATM_DELAY(6), ATM_NOTE_A5,
    ATM_DELAY(6), ATM_GOTO(7), ATM_GOTO(8), ATM_NOTE_E5, ATM_DELAY(6), ATM_NOTE_F5_, ATM_DELAY(6),
    ATM_NOTE_E5, ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(12), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_C6,
    ATM_GLIS(0x01), ATM_CUE(4), ATM_DELAY(4), ATM_GLIS_OFF, ATM_DELAY(2), ATM_NOTE_A5, ATM_DELAY(6),
    ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_D6, ATM_DELAY(6), ATM_NOTE_E6,
    ATM_DELAY(6), ATM_NOTE_F6_, ATM_DELAY(6), ATM_NOTE_E6, ATM_DELAY(6), ATM_NOTE_D6, ATM_DELAY(6),
    ATM_NOTE_C6, ATM_DELAY(4), ATM_NOTE_D6, ATM_DELAY(1), ATM_NOTE_C6, ATM_DELAY(3), ATM_NOTE_B5,
    ATM_DELAY(1), ATM_NOTE_C6, ATM_DELAY(3), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_B5, ATM_DELAY(6),
    ATM_NOTE_G5, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(1), ATM_NOTE_G5, ATM_DELAY(5), ATM_NOTE_D5,
    ATM_DELAY(6), ATM_NOTE_G5, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(1), ATM_NOTE_G5, ATM_DELAY(5),
    ATM_GOTO(6), ATM_NOTE_F6, ATM_GLIS(0x01), ATM_DELAY(4), ATM_GLIS_OFF, ATM_DELAY(2),
    ATM_NOTE_F6_, ATM_DELAY(6), ATM_NOTE_E6, ATM_DELAY(6), ATM_NOTE_F6_, ATM_DELAY(6), ATM_NOTE_D6,
    ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_GOTO(7), ATM_GOTO(8), ATM_NOTE_E5, ATM_DELAY(6),
    ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_E5, ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(4), ATM_NOTE_E5,
    ATM_DELAY(1), ATM_NOTE_D5, ATM_DELAY(3), ATM_NOTE_C5_, ATM_DELAY(1), ATM_NOTE_D5, ATM_DELAY(3),
    ATM_NOTE_A5, ATM_DELAY(6), ATM_GOTO(2),
    // track 3: ch0 bar 4
    ATM_NOTE_G5, ATM_DELAY(6), ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_E5, ATM_DELAY(6), ATM_NOTE_F5_,
    ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(6), ATM_NOTE_B4, ATM_DELAY(6), ATM_RETURN,
    // track 4: ch0 bar 8
    ATM_NOTE_D6, ATM_DELAY(6), ATM_NOTE_C6_, ATM_DELAY(6), ATM_NOTE_B5, ATM_DELAY(6), ATM_NOTE_A5,
    ATM_DELAY(6), ATM_NOTE_F5_, ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(6), ATM_RETURN,
    // track 5: ch0 bar 9
    ATM_NOTE_G5, ATM_DELAY(6), ATM_NOTE_B5, ATM_DELAY(6), ATM_NOTE_G5, ATM_DELAY(6), ATM_NOTE_F5_,
    ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_F5_, ATM_DELAY(6), ATM_RETURN,
    // track 6: ch0 bar 22
    ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_B5, ATM_DELAY(6), ATM_NOTE_C6_, ATM_DELAY(6), ATM_NOTE_D6,
    ATM_DELAY(6), ATM_NOTE_E6, ATM_DELAY(6), ATM_NOTE_F6_, ATM_DELAY(6), ATM_RETURN,
    // track 7: ch0 bar 24
    ATM_NOTE_E6, ATM_DELAY(6), ATM_NOTE_C6, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_G5,
    ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_C6, ATM_DELAY(6), ATM_RETURN,
    // track 8: ch0 bar 25
    ATM_NOTE_B5, ATM_DELAY(6), ATM_NOTE_A5, ATM_DELAY(6), ATM_NOTE_G5, ATM_DELAY(6), ATM_NOTE_F5_,
    ATM_DELAY(6), ATM_NOTE_E5, ATM_DELAY(6), ATM_NOTE_D5, ATM_DELAY(6), ATM_RETURN,
    // track 9: ch1 intro
    ATM_VOL(14), ATM_TREM(1, 5), ATM_ARP(0x70, 0x40), ATM_NOTE_D4, ATM_DELAY(36), ATM_GOTO(11),
    ATM_GOTO(10),
    // track 10: ch1 loop
    ATM_REPEAT(31, 11), ATM_GOTO(10),
    // track 11: ch1 bar 2
    ATM_NOTE_D4, ATM_DELAY(36), ATM_RETURN,
    // track 12: ch2 intro
    0x00, ATM_DELAY(36), ATM_DELAY(36), ATM_GOTO(13),
    // track 13: ch2 loop
    ATM_VOL(34), ATM_SL_VOL((uint8_t)-3), ATM_NOTE_D3, ATM_DELAY(6), 0x00, ATM_DELAY(6),
    ATM_NOTE_A3, ATM_DELAY(6), ATM_NOTE_D3, ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_A3,
    ATM_DELAY(6), ATM_GOTO(14), ATM_GOTO(15), ATM_GOTO(16), ATM_GOTO(15), ATM_GOTO(17),
    ATM_GOTO(18), ATM_GOTO(19), ATM_GOTO(15), ATM_GOTO(14), ATM_GOTO(15), ATM_GOTO(16),
    ATM_GOTO(15), ATM_GOTO(17), ATM_GOTO(18), ATM_GOTO(19), ATM_GOTO(15), ATM_GOTO(20),
    ATM_GOTO(21), ATM_GOTO(16), ATM_GOTO(15), ATM_GOTO(20), ATM_GOTO(18), ATM_GOTO(19),
    ATM_GOTO(15), ATM_GOTO(20), ATM_GOTO(21), ATM_GOTO(16), ATM_GOTO(15), ATM_GOTO(20),
    ATM_GOTO(18), ATM_GOTO(19), ATM_GOTO(13),
    // track 14: ch2 bar 4
    ATM_NOTE_G3, ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_D4, ATM_DELAY(6), ATM_NOTE_B2,
    ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_F3_, ATM_DELAY(6), ATM_RETURN,
    // track 15: ch2 bar 5
    ATM_NOTE_D3, ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_A3, ATM_DELAY(6), ATM_NOTE_D3,
    ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_A3, ATM_DELAY(6), ATM_RETURN,
    // track 16: ch2 bar 6
    ATM_NOTE_A3, ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_E4, ATM_DELAY(6), ATM_NOTE_A3,
    ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_E4, ATM_DELAY(6), ATM_RETURN,
    // track 17: ch2 bar 8
    ATM_NOTE_D3, ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_A3, ATM_DELAY(6), ATM_NOTE_A3,
    ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_E4, ATM_DELAY(6), ATM_RETURN,
    // track 18: ch2 bar 9
    ATM_NOTE_G3, ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_D4, ATM_DELAY(6), ATM_NOTE_D3,
    ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_A3, ATM_DELAY(6), ATM_RETURN,
    // track 19: ch2 bar 10
    ATM_NOTE_A3, ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_E4, ATM_DELAY(6), ATM_NOTE_D3,
    ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_A3, ATM_DELAY(6), ATM_RETURN,
    // track 20: ch2 bar 20
    ATM_NOTE_C3, ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_G3, ATM_DELAY(6), ATM_NOTE_C3,
    ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_G3, ATM_DELAY(6), ATM_RETURN,
    // track 21: ch2 bar 21
    ATM_NOTE_G3, ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_D4, ATM_DELAY(6), ATM_NOTE_G3,
    ATM_DELAY(6), 0x00, ATM_DELAY(6), ATM_NOTE_D4, ATM_DELAY(6), ATM_RETURN,
    // track 22: ch3 intro
    ATM_REPEAT(1, 24), ATM_GOTO(23),
    // track 23: ch3 loop
    ATM_REPEAT(6, 24), ATM_GOTO(25), ATM_REPEAT(6, 24), ATM_GOTO(25), ATM_REPEAT(6, 24),
    ATM_GOTO(25), ATM_REPEAT(6, 24), ATM_GOTO(25), ATM_GOTO(23),
    // track 24: ch3 bar 1
    ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-6), ATM_NOTE_C2, ATM_VOL(44), ATM_DELAY(6), ATM_NOISE_OFF,
    ATM_SL_VOL((uint8_t)-4), ATM_NOTE_C4, ATM_VOL(12), ATM_DELAY(6), ATM_NOTE_C4, ATM_VOL(20),
    ATM_DELAY(6), ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-6), ATM_NOTE_C2, ATM_VOL(44), ATM_DELAY(6),
    ATM_NOISE_OFF, ATM_SL_VOL((uint8_t)-4), ATM_NOTE_C4, ATM_VOL(12), ATM_DELAY(6), ATM_NOTE_C4,
    ATM_VOL(20), ATM_DELAY(6), ATM_RETURN,
    // track 25: ch3 bar 10
    ATM_NOISE(0x14), ATM_SL_VOL((uint8_t)-6), ATM_NOTE_C2, ATM_VOL(44), ATM_DELAY(6), ATM_NOISE_OFF,
    ATM_SL_VOL((uint8_t)-4), ATM_NOTE_C4, ATM_VOL(12), ATM_DELAY(6), ATM_NOTE_C4, ATM_VOL(20),
    ATM_DELAY(6), ATM_SL_VOL((uint8_t)-5), ATM_NOTE_C3, ATM_VOL(10), ATM_DELAY(3), ATM_NOTE_C3,
    ATM_VOL(14), ATM_DELAY(3), ATM_NOTE_C3, ATM_VOL(18), ATM_DELAY(3), ATM_NOTE_C3, ATM_VOL(22),
    ATM_DELAY(3), ATM_NOTE_C3, ATM_VOL(28), ATM_DELAY(3), ATM_NOTE_C3, ATM_VOL(34), ATM_DELAY(3),
    ATM_RETURN,
};
