// "RAIN ON GLASS" - LO-FI JAZZ, DB MAJOR. 739 bytes.
// Written by tools/songs/make_songs.py (nocturne.py): do not edit by hand.
// Tempo 30 (75 BPM: 24 ticks a beat, 4 beats a bar).
// 2 bars of intro, then 16 bars that loop (every channel's loop track ends with
// ATM_GOTO(itself), so the loop has no seam). CH0 pulse, CH1 square, CH2 saw, CH3 noise.
//
// The brief: "A late-night lo-fi jazz ballad at 75 BPM with a lazy triplet
// swing, in Db major. Lush extended harmony played as slow rolled
// arpeggios, each chord voiced by its colour tones above the bass:
// Dbmaj9 (Ab-C-Eb), Bbm9, Ebm9, Ab13 (Gb-C-F), Gbmaj7, then the bittersweet
// backdoor Gbm6, Fm7 to Bb7b9, and Ebm9 to D7, the tritone substitute
// that slides home to Db. A walking bass, a quarter a beat, chord tones on
// the strong beats and chromatic approach notes into every change. The
// pulse sings a muted-trumpet melody, laid back behind the beat, held notes
// with a slow vibrato, scoops up into the high ones, a blue note in the
// second chorus and a climb to a high Bb at its peak. Brushes: a swish on 2
// and 4, a swung ride pattern, and the faint crackle of a record. Form: two
// bars of intro, the 8-bar tune, and the tune again, embellished and higher."
// CH0 pulse: the melody. CH1 square: the chords. CH2 saw: the walking bass.
// CH3 noise: brushes and crackle.
#pragma once
#include "../atm/ATMcmds.h"

const uint8_t nocturne[] = {
    0x19,  // tracks
    0x00, 0x00,  // track 0: silent (unused channels; never called)
    0x01, 0x00,  // track 1: ch0 intro
    0x0A, 0x00,  // track 2: ch0 loop
    0xD7, 0x00,  // track 3: ch1 intro
    0xE6, 0x00,  // track 4: ch1 loop
    0x20, 0x01,  // track 5: ch1 bar 2
    0x27, 0x01,  // track 6: ch1 bar 5
    0x2E, 0x01,  // track 7: ch1 bar 9
    0x39, 0x01,  // track 8: ch1 bar 10
    0x44, 0x01,  // track 9: ch2 intro
    0x54, 0x01,  // track 10: ch2 loop
    0x76, 0x01,  // track 11: ch2 bar 3
    0x7F, 0x01,  // track 12: ch2 bar 4
    0x88, 0x01,  // track 13: ch2 bar 5
    0x91, 0x01,  // track 14: ch2 bar 6
    0x9A, 0x01,  // track 15: ch2 bar 7
    0xA3, 0x01,  // track 16: ch2 bar 8
    0xAC, 0x01,  // track 17: ch2 bar 9
    0xB5, 0x01,  // track 18: ch2 bar 10
    0xBE, 0x01,  // track 19: ch3 intro
    0xC4, 0x01,  // track 20: ch3 loop
    0xE6, 0x01,  // track 21: ch3 bar 1
    0x15, 0x02,  // track 22: ch3 bar 2
    0x48, 0x02,  // track 23: ch3 bar 3
    0x7B, 0x02,  // track 24: ch3 bar 4
    0x01,  // CH0 entry
    0x03,  // CH1 entry
    0x09,  // CH2 entry
    0x13,  // CH3 entry
    // track 0: silent (unused channels; never called)
    ATM_STOP_CHAN,
    // track 1: ch0 intro
    ATM_SET_TEMPO(30), 0x00, ATM_DELAY(64), ATM_DELAY(32), ATM_DELAY(64), ATM_DELAY(32),
    ATM_GOTO(2),
    // track 2: ch0 loop
    ATM_SET_TEMPO(30), ATM_VOL(28), ATM_SL_VOL(0), ATM_VIB(2, 5), 0x00, ATM_CUE(1), ATM_DELAY(16),
    ATM_NOTE_G5_, ATM_DELAY(8), ATM_NOTE_C6, ATM_DELAY(48), ATM_NOTE_A5_, ATM_DELAY(16),
    ATM_NOTE_G5_, ATM_DELAY(8), ATM_NOTE_F5, ATM_DELAY(48), ATM_NOTE_D5_, ATM_DELAY(16),
    ATM_NOTE_F5, ATM_DELAY(8), ATM_NOTE_G5_, ATM_DELAY(24), ATM_NOTE_F5_, ATM_DELAY(16),
    ATM_NOTE_F5, ATM_DELAY(8), ATM_NOTE_D5_, ATM_DELAY(16), ATM_NOTE_C5_, ATM_DELAY(8),
    ATM_NOTE_A5_, ATM_DELAY(48), ATM_NOTE_A5_, ATM_GLIS(0x02), ATM_DELAY(6), ATM_GLIS_OFF,
    ATM_DELAY(18), ATM_NOTE_A5_, ATM_DELAY(16), ATM_NOTE_G5_, ATM_DELAY(8), ATM_NOTE_F5,
    ATM_DELAY(48), 0x00, ATM_DELAY(16), ATM_NOTE_A5_, ATM_DELAY(8), ATM_NOTE_C6_, ATM_DELAY(16),
    ATM_NOTE_D6_, ATM_DELAY(8), ATM_NOTE_F6, ATM_DELAY(48), ATM_NOTE_D6_, ATM_DELAY(16),
    ATM_NOTE_C6_, ATM_DELAY(8), ATM_NOTE_A5, ATM_DELAY(48), ATM_NOTE_G5_, ATM_DELAY(24),
    ATM_NOTE_G5_, ATM_DELAY(16), ATM_NOTE_F5, ATM_DELAY(8), ATM_NOTE_C6, ATM_DELAY(24), ATM_NOTE_B5,
    ATM_DELAY(16), ATM_NOTE_G5_, ATM_DELAY(8), ATM_NOTE_F5, ATM_DELAY(24), ATM_NOTE_F5_,
    ATM_DELAY(16), ATM_NOTE_F5, ATM_DELAY(8), ATM_NOTE_D5_, ATM_DELAY(24), ATM_NOTE_C6,
    ATM_DELAY(16), ATM_NOTE_A5, ATM_DELAY(8), ATM_NOTE_F5_, ATM_DELAY(24), 0x00, ATM_CUE(2),
    ATM_DELAY(8), ATM_NOTE_G5_, ATM_DELAY(8), ATM_NOTE_A5_, ATM_DELAY(8), ATM_NOTE_C6,
    ATM_DELAY(24), ATM_NOTE_D6_, ATM_DELAY(16), ATM_NOTE_C6_, ATM_DELAY(8), ATM_NOTE_C6,
    ATM_DELAY(16), ATM_NOTE_G5_, ATM_DELAY(8), ATM_NOTE_F5, ATM_DELAY(24), ATM_NOTE_E5,
    ATM_DELAY(16), ATM_NOTE_F5, ATM_DELAY(8), ATM_NOTE_G5_, ATM_DELAY(16), ATM_NOTE_C6,
    ATM_DELAY(8), ATM_NOTE_C6_, ATM_DELAY(24), ATM_NOTE_D6_, ATM_DELAY(16), ATM_NOTE_C6_,
    ATM_DELAY(8), ATM_NOTE_A5_, ATM_DELAY(16), ATM_NOTE_F5_, ATM_DELAY(8), ATM_NOTE_F5,
    ATM_DELAY(16), ATM_NOTE_D5_, ATM_DELAY(8), ATM_NOTE_C5_, ATM_DELAY(24), ATM_NOTE_C5,
    ATM_DELAY(16), ATM_NOTE_D5_, ATM_DELAY(8), ATM_NOTE_F5, ATM_DELAY(16), ATM_NOTE_G5_,
    ATM_DELAY(8), ATM_NOTE_A5_, ATM_GLIS(0x02), ATM_DELAY(6), ATM_GLIS_OFF, ATM_DELAY(18),
    ATM_NOTE_A5_, ATM_DELAY(24), ATM_NOTE_C6_, ATM_DELAY(16), ATM_NOTE_F6, ATM_DELAY(8),
    ATM_NOTE_G6_, ATM_GLIS(0x02), ATM_DELAY(6), ATM_GLIS_OFF, ATM_DELAY(42), ATM_NOTE_G6_,
    ATM_DELAY(16), ATM_NOTE_F6, ATM_DELAY(8), ATM_NOTE_G6_, ATM_DELAY(16), ATM_NOTE_F6_,
    ATM_DELAY(8), ATM_NOTE_D6_, ATM_DELAY(24), ATM_NOTE_C6_, ATM_DELAY(16), ATM_NOTE_A5,
    ATM_DELAY(8), ATM_NOTE_A5_, ATM_DELAY(24), ATM_NOTE_G5_, ATM_DELAY(16), ATM_NOTE_G5,
    ATM_DELAY(8), ATM_NOTE_F5, ATM_DELAY(24), ATM_NOTE_B5, ATM_DELAY(16), ATM_NOTE_D6, ATM_DELAY(8),
    ATM_NOTE_F6, ATM_DELAY(24), ATM_NOTE_D6_, ATM_DELAY(16), ATM_NOTE_C6_, ATM_DELAY(8),
    ATM_NOTE_A5_, ATM_DELAY(24), ATM_NOTE_C6, ATM_DELAY(16), ATM_NOTE_A5, ATM_DELAY(8),
    ATM_NOTE_F5_, ATM_DELAY(24), ATM_GOTO(2),
    // track 3: ch1 intro
    ATM_VOL(15), ATM_TREM(1, 11), ATM_ARP(0x43, 0x22), ATM_NOTE_G4_, ATM_DELAY(64), ATM_DELAY(32),
    ATM_GOTO(5), ATM_GOTO(4),
    // track 4: ch1 loop
    ATM_ARP(0x43, 0x22), ATM_NOTE_G4_, ATM_DELAY(64), ATM_DELAY(32), ATM_ARP(0x34, 0x22),
    ATM_NOTE_F4, ATM_DELAY(64), ATM_DELAY(32), ATM_GOTO(6), ATM_GOTO(5), ATM_GOTO(6),
    ATM_ARP(0x42, 0x22), ATM_NOTE_A4, ATM_DELAY(64), ATM_DELAY(32), ATM_GOTO(7), ATM_GOTO(8),
    ATM_ARP(0x43, 0x22), ATM_NOTE_G4_, ATM_DELAY(64), ATM_DELAY(32), ATM_ARP(0x34, 0x22),
    ATM_NOTE_F4, ATM_DELAY(64), ATM_DELAY(32), ATM_GOTO(6), ATM_GOTO(5), ATM_GOTO(6),
    ATM_ARP(0x42, 0x22), ATM_NOTE_A4, ATM_DELAY(64), ATM_DELAY(32), ATM_GOTO(7), ATM_GOTO(8),
    ATM_GOTO(4),
    // track 5: ch1 bar 2
    ATM_ARP(0x65, 0x22), ATM_NOTE_F4_, ATM_DELAY(64), ATM_DELAY(32), ATM_RETURN,
    // track 6: ch1 bar 5
    ATM_ARP(0x34, 0x22), ATM_NOTE_A4_, ATM_DELAY(64), ATM_DELAY(32), ATM_RETURN,
    // track 7: ch1 bar 9
    ATM_ARP(0x43, 0x22), ATM_NOTE_G4_, ATM_DELAY(48), ATM_ARP(0x33, 0x22), ATM_NOTE_G4_,
    ATM_DELAY(48), ATM_RETURN,
    // track 8: ch1 bar 10
    ATM_ARP(0x34, 0x22), ATM_NOTE_A4_, ATM_DELAY(48), ATM_ARP(0x65, 0x22), ATM_NOTE_C5,
    ATM_DELAY(48), ATM_RETURN,
    // track 9: ch2 intro
    ATM_VOL(40), ATM_SL_VOL((uint8_t)-1), ATM_NOTE_C3_, ATM_DELAY(48), ATM_NOTE_G2_, ATM_DELAY(48),
    ATM_NOTE_G2_, ATM_DELAY(48), ATM_NOTE_D3_, ATM_DELAY(24), ATM_NOTE_D3, ATM_DELAY(24),
    ATM_GOTO(10),
    // track 10: ch2 loop
    ATM_GOTO(11), ATM_GOTO(12), ATM_GOTO(13), ATM_GOTO(14), ATM_GOTO(15), ATM_GOTO(16),
    ATM_GOTO(17), ATM_GOTO(18), ATM_GOTO(11), ATM_GOTO(12), ATM_GOTO(13), ATM_GOTO(14),
    ATM_GOTO(15), ATM_GOTO(16), ATM_GOTO(17), ATM_GOTO(18), ATM_GOTO(10),
    // track 11: ch2 bar 3
    ATM_NOTE_C3_, ATM_DELAY(24), ATM_NOTE_F3, ATM_DELAY(24), ATM_NOTE_G3_, ATM_DELAY(24),
    ATM_NOTE_A3, ATM_DELAY(24), ATM_RETURN,
    // track 12: ch2 bar 4
    ATM_NOTE_A3_, ATM_DELAY(24), ATM_NOTE_G3_, ATM_DELAY(24), ATM_NOTE_F3, ATM_DELAY(24),
    ATM_NOTE_E3, ATM_DELAY(24), ATM_RETURN,
    // track 13: ch2 bar 5
    ATM_NOTE_D3_, ATM_DELAY(24), ATM_NOTE_F3, ATM_DELAY(24), ATM_NOTE_F3_, ATM_DELAY(24),
    ATM_NOTE_G3, ATM_DELAY(24), ATM_RETURN,
    // track 14: ch2 bar 6
    ATM_NOTE_G2_, ATM_DELAY(24), ATM_NOTE_C3, ATM_DELAY(24), ATM_NOTE_D3_, ATM_DELAY(24),
    ATM_NOTE_F3, ATM_DELAY(24), ATM_RETURN,
    // track 15: ch2 bar 7
    ATM_NOTE_F3_, ATM_DELAY(24), ATM_NOTE_A3_, ATM_DELAY(24), ATM_NOTE_C4_, ATM_DELAY(24),
    ATM_NOTE_A3_, ATM_DELAY(24), ATM_RETURN,
    // track 16: ch2 bar 8
    ATM_NOTE_F3_, ATM_DELAY(24), ATM_NOTE_A3, ATM_DELAY(24), ATM_NOTE_C3_, ATM_DELAY(24),
    ATM_NOTE_E3, ATM_DELAY(24), ATM_RETURN,
    // track 17: ch2 bar 9
    ATM_NOTE_F3, ATM_DELAY(24), ATM_NOTE_C3, ATM_DELAY(24), ATM_NOTE_A2_, ATM_DELAY(24),
    ATM_NOTE_D3, ATM_DELAY(24), ATM_RETURN,
    // track 18: ch2 bar 10
    ATM_NOTE_D3_, ATM_DELAY(24), ATM_NOTE_A2_, ATM_DELAY(24), ATM_NOTE_D3, ATM_DELAY(24),
    ATM_NOTE_G2_, ATM_DELAY(24), ATM_RETURN,
    // track 19: ch3 intro
    ATM_GOTO(21), ATM_GOTO(22), ATM_GOTO(20),
    // track 20: ch3 loop
    ATM_GOTO(23), ATM_GOTO(24), ATM_GOTO(21), ATM_GOTO(22), ATM_GOTO(23), ATM_GOTO(24),
    ATM_GOTO(21), ATM_GOTO(22), ATM_GOTO(23), ATM_GOTO(24), ATM_GOTO(21), ATM_GOTO(22),
    ATM_GOTO(23), ATM_GOTO(24), ATM_GOTO(21), ATM_GOTO(22), ATM_GOTO(20),
    // track 21: ch3 bar 1
    ATM_SL_VOL((uint8_t)-1), ATM_NOTE_D6, ATM_VOL(10), ATM_DELAY(10), ATM_SL_VOL((uint8_t)-5),
    ATM_NOTE_C4, ATM_VOL(5), ATM_DELAY(14), ATM_SL_VOL((uint8_t)-1), ATM_NOTE_C5_, ATM_VOL(14),
    ATM_DELAY(16), ATM_NOTE_D6, ATM_VOL(6), ATM_DELAY(8), ATM_NOTE_D6, ATM_VOL(10), ATM_DELAY(10),
    ATM_SL_VOL((uint8_t)-5), ATM_NOTE_C4, ATM_VOL(5), ATM_DELAY(6), ATM_NOTE_C4, ATM_VOL(5),
    ATM_DELAY(8), ATM_SL_VOL((uint8_t)-1), ATM_NOTE_C5_, ATM_VOL(14), ATM_DELAY(16), ATM_NOTE_D6,
    ATM_VOL(6), ATM_DELAY(8), ATM_RETURN,
    // track 22: ch3 bar 2
    ATM_SL_VOL((uint8_t)-1), ATM_NOTE_D6, ATM_VOL(10), ATM_DELAY(24), ATM_NOTE_C5_, ATM_VOL(14),
    ATM_DELAY(6), ATM_SL_VOL((uint8_t)-5), ATM_NOTE_C4, ATM_VOL(5), ATM_DELAY(10),
    ATM_SL_VOL((uint8_t)-1), ATM_NOTE_D6, ATM_VOL(6), ATM_DELAY(8), ATM_NOTE_D6, ATM_VOL(10),
    ATM_DELAY(13), ATM_SL_VOL((uint8_t)-5), ATM_NOTE_C4, ATM_VOL(5), ATM_DELAY(11),
    ATM_SL_VOL((uint8_t)-1), ATM_NOTE_C5_, ATM_VOL(14), ATM_DELAY(11), ATM_SL_VOL((uint8_t)-5),
    ATM_NOTE_C4, ATM_VOL(5), ATM_DELAY(5), ATM_SL_VOL((uint8_t)-1), ATM_NOTE_D6, ATM_VOL(6),
    ATM_DELAY(8), ATM_RETURN,
    // track 23: ch3 bar 3
    ATM_SL_VOL((uint8_t)-1), ATM_NOTE_D6, ATM_VOL(10), ATM_DELAY(5), ATM_SL_VOL((uint8_t)-5),
    ATM_NOTE_C4, ATM_VOL(5), ATM_DELAY(19), ATM_SL_VOL((uint8_t)-1), ATM_NOTE_C5_, ATM_VOL(14),
    ATM_DELAY(16), ATM_NOTE_D6, ATM_VOL(6), ATM_DELAY(8), ATM_NOTE_D6, ATM_VOL(10), ATM_DELAY(6),
    ATM_SL_VOL((uint8_t)-5), ATM_NOTE_C4, ATM_VOL(5), ATM_DELAY(18), ATM_SL_VOL((uint8_t)-1),
    ATM_NOTE_C5_, ATM_VOL(14), ATM_DELAY(5), ATM_SL_VOL((uint8_t)-5), ATM_NOTE_C4, ATM_VOL(5),
    ATM_DELAY(11), ATM_SL_VOL((uint8_t)-1), ATM_NOTE_D6, ATM_VOL(6), ATM_DELAY(8), ATM_RETURN,
    // track 24: ch3 bar 4
    ATM_SL_VOL((uint8_t)-1), ATM_NOTE_D6, ATM_VOL(10), ATM_DELAY(19), ATM_SL_VOL((uint8_t)-5),
    ATM_NOTE_C4, ATM_VOL(5), ATM_DELAY(5), ATM_SL_VOL((uint8_t)-1), ATM_NOTE_C5_, ATM_VOL(14),
    ATM_DELAY(16), ATM_NOTE_D6, ATM_VOL(6), ATM_DELAY(8), ATM_NOTE_D6, ATM_VOL(10), ATM_DELAY(18),
    ATM_SL_VOL((uint8_t)-5), ATM_NOTE_C4, ATM_VOL(5), ATM_DELAY(6), ATM_SL_VOL((uint8_t)-1),
    ATM_NOTE_C5_, ATM_VOL(14), ATM_DELAY(16), ATM_NOTE_D6, ATM_VOL(6), ATM_DELAY(3),
    ATM_SL_VOL((uint8_t)-5), ATM_NOTE_C4, ATM_VOL(5), ATM_DELAY(5), ATM_RETURN,
};
