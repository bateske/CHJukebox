// ATMlib for the CHGame: Arduboy Tracker Music, the four-channel chiptune
// synth and song format of the Arduboy (T-arg/ATMlib: Davey Taylor's
// ATMsynth, after his Squawk; Joeri Gantois' song format), ported to the
// CH32X035 and its one buzzer pin. BSD-3-Clause: LICENSE-ATMlib.
//
// Songs are the Arduboy's byte for byte. ATMcmds.h is the original, so a
// song.h written for an Arduboy sketch plays here as it is, and the
// songwriting guide in ATMlib's README holds: CH0 pulse (25 %), CH1
// square, CH2 saw, CH3 noise; 16 ticks a beat at tempo t is t x 3.75 BPM;
// self-looping tracks; ATM_CUE for the sketch.
//
//   #include "src/atm/ATMlib.h"
//   ATMsynth ATM;
//   ATM.play(song);                         // in setup()
//   ATM.pump(micros());                     // in loop() (only the simulator needs it)
//
// The playroutine (ATMlib.cpp) is ATMlibimpl.h's, statement for statement,
// with every effect and function compiled in. What changed:
//
//   - Output. The Arduboy mixes at 15.6 kHz in a Timer4 interrupt into an
//     8-bit PWM. Here the buzzer's timer (whichever the board package's
//     variant maps PIN_BUZZER to: TIM1 on Rev0, TIM2 on Rev1) runs it as an
//     8-bit PWM at 187.5 kHz (ARR 255 at 48 MHz), and its repetition
//     counter raises the update interrupt every 12 periods: 15,625 samples
//     a second, the rate ATMlib's tempo maths was written for. The mixer
//     and the playroutine run from SRAM. Nothing names a board: one source
//     builds for each board the installed package has.
//   - Tuning. Note values are the Arduboy's (ATM_NOTE_A4 is 1760 in the
//     table, slides and vibrato move it in the same units), but the
//     oscillators run so that the table reads as quarter-Hz: ATM_NOTE_A4
//     sounds at 440 Hz, every note at concert pitch. The Arduboy plays the
//     same songs about 75 cents flat.
//   - Tempo. A tick every 15625 / tempo samples, kept as a fraction, so
//     ticks per second are exactly the tempo value.
//   - A silent voice is silent: a note-off (note 0), a cut or a slide to
//     the bottom holds the voice at the centre instead of a DC level, so
//     rests do not click.
//   - Safety. A track that calls past the seven-deep stack, names a track
//     the song does not have, or runs 64 commands without a delay stops
//     its channel instead of running wild in an interrupt.
//   - The mix is summed in full and clipped, and the DRIVE setting scales
//     it before the clip: the buzzer has one pin and no amplifier.
//   - The BEEP engine: the same songs on the buzzer as plain square waves
//     (the CHGame library's way), for a buzzer that does not take to PWM
//     audio. The voices take turns, a volume becomes a pulse width.
//
// For the visualiser it adds: the score's state (atm::Score), the
// oscillators (snapshot()), a tick count, the output's peak, and
// atm::Score/atm::scoreTick to run a second copy of a song ahead of the
// sound (what the piano roll shows before it plays).
#pragma once
#include <stdint.h>
#ifndef PROGMEM
#define PROGMEM                         // ATM_SFX_TRACK's (flash is mapped on the CH32)
#endif
#include "ATMcmds.h"

#define CH_ZERO             0
#define CH_ONE              1
#define CH_TWO              2
#define CH_THREE            3

namespace atm {

static const uint16_t RATE = 15625;     // samples a second
// Oscillator step per sample for a note-table value: the table is
// quarter-Hz, the phase a 32-bit fraction of a cycle (2^32 / 4 / 15625).
static const uint32_t STEP = 68719;

// One voice as the mixer sees it.
struct Osc {
    uint32_t phase;     // fraction of a cycle (unused by the noise voice)
    uint32_t step;      // freq x STEP
    uint16_t freq;      // note-table units; the noise voice keeps its LFSR here
    uint8_t vol;        // 0-63 as written (a volume slide past 63 is not clamped, as on the Arduboy)
};

// One channel of the playroutine: ATMlibimpl.h's ch_t, plus `trig` and `fx`.
struct Chan {
    const uint8_t *ptr;
    uint8_t note;
    uint16_t stackPointer[7];
    uint8_t stackCounter[7];
    uint8_t stackTrack[7];
    uint8_t stackIndex;
    uint8_t repeatPoint;
    uint16_t delay;
    uint8_t counter;
    uint8_t track;
    uint16_t freq;
    uint8_t vol;
    int8_t volFreSlide;
    uint8_t volFreConfig;
    uint8_t volFreCount;
    uint8_t arpNotes;
    uint8_t arpTiming;
    uint8_t arpCount;
    uint8_t reConfig;
    uint8_t reCount;
    int8_t transConfig;
    uint8_t treviDepth;
    uint8_t treviConfig;
    uint8_t treviCount;
    int8_t glisConfig;
    uint8_t glisCount;
    // Not in the original: what this tick did, for the visualiser.
    uint8_t trig;       // a note started this tick
    uint8_t fx;         // the last effect command run this tick (0x40-0x9F), 0 if none
};

// A song being played: four channels and the song-wide state.
struct Score {
    Chan ch[4];
    const uint8_t *trackList;   // little-endian offsets
    const uint8_t *trackBase;
    uint8_t trackCount;
    uint8_t tickRate;           // the tempo: ticks a second
    uint8_t active;             // bit n: channel n has not stopped
    uint8_t cue;                // the last ATM_CUE
    bool ended;                 // every channel stopped and no GOTO_ADV restart
};

// The note table (ATMlibimpl.h's): 0 = silence, 1 = C2 ... 63 = D7.
uint16_t noteFreq(int note);

// Load a song into a score (the header, the entry tracks), tempo 25.
void scoreLoad(Score &s, const uint8_t *song);
// One tick of a whole score with nothing muted, no effects and no pause,
// writing the voices' frequency and volume into o[4] as the synth would.
// The visualiser's look-ahead copy runs on this.
void scoreTick(Score &s, Osc *o);
// Whether a voice with this channel state sounds (note on, a pitch above a
// few Hz). The noise channel sounds whenever its volume is up.
inline bool gate(const Chan &c, uint8_t n) {
    if (n == 3) return c.vol != 0;
    return c.note && c.freq >= 32 && c.vol;
}

enum Engine : uint8_t { SYNTH, BEEP };

}  // namespace atm

class ATMsynth {
public:
    ATMsynth() {}

    // ATMlib's API (all of it compiled in).
    void play(const uint8_t *song);
    void playPause();
    void pause();
    void resume();
    void stop();
    void muteChannel(uint8_t ch);
    void unMuteChannel(uint8_t ch);
    void playSfx(const uint8_t *track, uint8_t ch);
    void setTempo(uint8_t t);
    uint8_t check();                // the last cue, cleared (0: none since the last read)
    uint8_t check(uint8_t id);      // 1 if the last cue is id (not cleared)

    // The CHGame port's.
    void pump(uint32_t us);         // the simulator renders up to `us` of its clock; nothing on the board
    bool playing() const;           // a song is loaded and has not ended (paused counts)
    bool paused() const;
    uint8_t muted() const;          // bit n: channel n muted
    uint32_t ticks() const;         // score ticks since play() (not while paused)
    const atm::Score &score() const;
    void snapshot(atm::Osc out[4]) const;   // the four voices, taken together
    uint8_t sfxChannel() const;     // the channel an effect holds, 0xFF: none
    uint16_t peak();                // the mix's largest swing since the last call (0-220)

    void setDrive(uint8_t sixteenths);   // mix gain before the clip: 16 = x1 (default 24)
    uint8_t drive() const;
    void setEngine(uint8_t e);      // atm::SYNTH (the PWM mix) or atm::BEEP (square waves)
    uint8_t engine() const;
    void led(bool on);              // the board's LED (PIN_LED): the visualiser flashes it on the beat

    // The mix, one sample at a time (the simulator and the PC renderer):
    // 0-255, 128 the centre.
    static uint8_t render();
};
