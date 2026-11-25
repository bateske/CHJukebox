// ATMlib for the CHGame: the playroutine, the mixer and the output. See
// ATMlib.h for what changed from the Arduboy's ATMlibimpl.h; the
// playroutine below follows it statement for statement, so the comments
// mark only where it does not.
//
// Three builds: the board (the buzzer's timer and SysTick interrupts drive it), the
// simulator (CHSIM: pump(us) renders the samples up to the sketch's clock, and
// $ATM_WAV, if set, names a WAV file that gets every sample), and the PC
// tools (ATM_HOST: render() only).
#include "ATMlib.h"
#include <string.h>

#if defined(__riscv) && !defined(CHSIM) && !defined(ATM_HOST)
#define ATM_DEVICE 1
#include <Arduino.h>
#include <chgame/RamFunc.h>
#else
#define ATM_DEVICE 0
#define RAMFUNC(name)
#endif
#ifdef CHSIM
#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#endif

namespace atm {

// The note table: ATMlibimpl.h's, all six octaves.
static const uint16_t NOTES[64] = {
    0,
    262,  277,  294,  311,  330,  349,  370,  392,  415,  440,  466,  494,
    523,  554,  587,  622,  659,  698,  740,  784,  831,  880,  932,  988,
    1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
    2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
    4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902,
    8372, 8870, 9397,
};

// Out of 1..63 is silence (the Arduboy reads past the table).
static inline uint16_t nf(int n) { return (unsigned)n < 64u ? NOTES[n] : 0; }
uint16_t noteFreq(int note) { return nf(note); }

static inline const uint8_t *trackPtr(const Score &s, uint8_t t) {
    const uint8_t *e = s.trackList + 2 * t;
    return s.trackBase + (uint16_t)(e[0] | (e[1] << 8));
}

void scoreLoad(Score &s, const uint8_t *song) {
    memset(&s, 0, sizeof s);
    s.tickRate = 25;
    s.active = 0x0F;
    s.trackCount = song[0];
    s.trackList = song + 1;
    s.trackBase = song + 1 + 2 * s.trackCount + 4;
    for (int n = 0; n < 4; n++) {
        uint8_t t = song[1 + 2 * s.trackCount + n];
        s.ch[n].ptr = t < s.trackCount ? trackPtr(s, t) : nullptr;
        if (!s.ch[n].ptr) { s.ch[n].delay = 0xFFFF; s.active &= (uint8_t)~(1 << n); }
    }
    s.ch[3].freq = 0x0001;
}

enum : uint8_t { STOPPED = 1 };

static RAMFUNC(atmStop) void halt(Chan *ch) {
    ch->vol = 0;
    ch->delay = 0xFFFF;
}

// One tick of one channel: the effects, then the commands until a delay.
// `o` is where a noise retrigger reseeds the LFSR (o[3]). Returns STOPPED
// when the channel stopped this tick.
static RAMFUNC(atmStep) uint8_t step(Score &s, Chan *ch, Osc *o) {
    uint8_t r = 0;
    ch->trig = 0;
    ch->fx = 0;

    // Noise retriggering: reseed the LFSR.
    if (ch->reConfig) {
        if (ch->reCount >= (ch->reConfig & 0x03)) {
            o[3].freq = nf(ch->reConfig >> 2);
            ch->reCount = 0;
        } else ch->reCount++;
    }

    // Glissando.
    if (ch->glisConfig) {
        if (ch->glisCount >= (ch->glisConfig & 0x7F)) {
            if (ch->glisConfig & 0x80) ch->note -= 1;
            else ch->note += 1;
            if (ch->note < 1) ch->note = 1;
            else if (ch->note > 63) ch->note = 63;
            ch->freq = nf(ch->note);
            ch->glisCount = 0;
        } else ch->glisCount++;
    }

    // Volume / frequency slide. (The original's dangling else: a frequency
    // is clamped to 9397, a volume is never clamped at 63.)
    if (ch->volFreSlide) {
        if (!ch->volFreCount) {
            int16_t vf = (ch->volFreConfig & 0x40) ? (int16_t)ch->freq : (int16_t)ch->vol;
            vf += ch->volFreSlide;
            if (!(ch->volFreConfig & 0x80)) {
                if (vf < 0) vf = 0;
                else if ((ch->volFreConfig & 0x40) && vf > 9397) vf = 9397;
            }
            if (ch->volFreConfig & 0x40) ch->freq = (uint16_t)vf;
            else ch->vol = (uint8_t)vf;
        }
        if (ch->volFreCount++ >= (ch->volFreConfig & 0x3F)) ch->volFreCount = 0;
    }

    // Arpeggio or note cut. (The transposition is added a second time,
    // as on the Arduboy: songs avoid arpeggio + transposition.)
    if (ch->arpNotes && ch->note) {
        if ((ch->arpCount & 0x1F) < (ch->arpTiming & 0x1F)) ch->arpCount++;
        else {
            if ((ch->arpCount & 0xE0) == 0x00) ch->arpCount = 0x20;
            else if ((ch->arpCount & 0xE0) == 0x20 && !(ch->arpTiming & 0x40) && ch->arpNotes != 0xFF)
                ch->arpCount = 0x40;
            else ch->arpCount = 0x00;
            uint8_t arpNote = ch->note;
            if ((ch->arpCount & 0xE0) != 0x00) {
                if (ch->arpNotes == 0xFF) arpNote = 0;
                else arpNote += (ch->arpNotes >> 4);
            }
            if ((ch->arpCount & 0xE0) == 0x40) arpNote += (ch->arpNotes & 0x0F);
            ch->freq = nf(arpNote + ch->transConfig);
        }
    }

    // Tremolo or vibrato (the same clamps as the slides).
    if (ch->treviDepth) {
        int16_t vt = (ch->treviConfig & 0x40) ? (int16_t)ch->freq : (int16_t)ch->vol;
        vt = (ch->treviCount & 0x80) ? (int16_t)(vt + ch->treviDepth) : (int16_t)(vt - ch->treviDepth);
        if (vt < 0) vt = 0;
        else if ((ch->treviConfig & 0x40) && vt > 9397) vt = 9397;
        if (ch->treviConfig & 0x40) ch->freq = (uint16_t)vt;
        else ch->vol = (uint8_t)vt;
        if ((ch->treviCount & 0x1F) < (ch->treviConfig & 0x1F)) ch->treviCount++;
        else ch->treviCount = (ch->treviCount & 0x80) ? 0 : 0x80;
    }

    if (ch->delay) {
        if (ch->delay != 0xFFFF) ch->delay--;
        return r;
    }
    // Not in the original: 64 commands without a delay stop the channel
    // (a track that only calls itself would hang the interrupt).
    uint8_t guard = 64;
    do {
        if (!--guard) { halt(ch); return STOPPED; }
        uint8_t cmd = *ch->ptr++;
        if (cmd < 64) {
            // 0 ... 63: note on / off
            ch->note = cmd;
            if (cmd) ch->note += ch->transConfig;
            ch->freq = nf(ch->note);
            if (!ch->volFreConfig) ch->vol = ch->reCount;
            if (ch->arpTiming & 0x20) ch->arpCount = 0;    // arpeggio retrigger
            ch->trig = 1;
        } else if (cmd < 160) {
            // 64 ... 159: effects. Unknown ones take no parameters, as on the Arduboy.
            ch->fx = cmd;
            switch (cmd - 64) {
            case 0:                                         // set volume
                ch->vol = *ch->ptr++;
                ch->reCount = ch->vol;
                break;
            case 1: case 4:                                 // slide volume / frequency on
                ch->volFreSlide = (int8_t)*ch->ptr++;
                ch->volFreConfig = (cmd - 64) == 1 ? 0x00 : 0x40;
                break;
            case 2: case 5:                                 // ... advanced
                ch->volFreSlide = (int8_t)*ch->ptr++;
                ch->volFreConfig = *ch->ptr++;
                if ((cmd - 64) == 5) ch->volFreConfig |= 0x40;
                break;
            case 3: case 6:                                 // slide off
                ch->volFreSlide = 0;
                break;
            case 7:                                         // arpeggio
                ch->arpNotes = *ch->ptr++;
                ch->arpTiming = *ch->ptr++;
                break;
            case 8:
                ch->arpNotes = 0;
                break;
            case 9:                                         // noise retrigger
                ch->reConfig = *ch->ptr++;
                break;
            case 10:
                ch->reConfig = 0;
                break;
            case 11:                                        // add transposition
                ch->transConfig = (int8_t)(ch->transConfig + (int8_t)*ch->ptr++);
                break;
            case 12:                                        // set transposition
                ch->transConfig = (int8_t)*ch->ptr++;
                break;
            case 13:
                ch->transConfig = 0;
                break;
            case 14: case 16:                               // tremolo / vibrato
                ch->treviDepth = *ch->ptr++;
                ch->treviConfig = (uint8_t)(*ch->ptr++ + ((cmd - 64) == 14 ? 0x00 : 0x40));
                break;
            case 15: case 17:
                ch->treviDepth = 0;
                break;
            case 18:                                        // glissando
                ch->glisConfig = (int8_t)*ch->ptr++;
                break;
            case 19:
                ch->glisConfig = 0;
                break;
            case 20:                                        // note cut
                ch->arpNotes = 0xFF;
                ch->arpTiming = *ch->ptr++;
                break;
            case 21:
                ch->arpNotes = 0;
                break;
            case 23:                                        // cue
                s.cue = *ch->ptr++;
                break;
            case 92:                                        // add tempo
                s.tickRate = (uint8_t)(s.tickRate + *ch->ptr++);
                break;
            case 93:                                        // set tempo
                s.tickRate = *ch->ptr++;
                break;
            case 94:                                        // goto advanced: the restart tracks
                for (int i = 0; i < 4; i++) s.ch[i].repeatPoint = *ch->ptr++;
                break;
            case 95:                                        // stop this channel
                halt(ch);
                r = STOPPED;
                break;
            }
        } else if (cmd < 224) {
            ch->delay = (uint16_t)(cmd - 159);              // 160 ... 223: delay 1-64
        } else if (cmd == 224) {
            // Long delay: a 7-bit-per-byte number + 65. The Arduboy's
            // read_vle() never moves past it (it steps its own argument,
            // not the song pointer); here it does, as the README describes.
            uint16_t q = 0;
            uint8_t d;
            do {
                d = *ch->ptr++;
                q = (uint16_t)((q << 7) | (d & 0x7F));
            } while (d & 0x80);
            ch->delay = (uint16_t)(q + 65);
        } else if (cmd < 252) {
            // 225 ... 251: reserved
        } else if (cmd == 252 || cmd == 253) {
            // call (repeatedly)
            uint8_t newCounter = cmd == 252 ? 0 : *ch->ptr++;
            uint8_t newTrack = *ch->ptr++;
            if (newTrack >= s.trackCount) { halt(ch); return STOPPED; }    // (not in the original)
            if (newTrack != ch->track) {
                if (ch->stackIndex >= 7) { halt(ch); return STOPPED; }       // (not in the original)
                ch->stackCounter[ch->stackIndex] = ch->counter;
                ch->stackTrack[ch->stackIndex] = ch->track;
                ch->stackPointer[ch->stackIndex] = (uint16_t)(ch->ptr - s.trackBase);
                ch->stackIndex++;
                ch->track = newTrack;
            }
            ch->counter = newCounter;
            ch->ptr = trackPtr(s, ch->track);
        } else if (cmd == 254) {
            // return
            if (ch->counter > 0 || ch->stackIndex == 0) {
                if (ch->counter) ch->counter--;
                ch->ptr = trackPtr(s, ch->track);       // repeat the track
            } else {
                ch->stackIndex--;
                ch->ptr = ch->stackPointer[ch->stackIndex] + s.trackBase;
                ch->counter = ch->stackCounter[ch->stackIndex];
                ch->track = ch->stackTrack[ch->stackIndex];
            }
        }
    } while (ch->delay == 0);
    if (ch->delay != 0xFFFF) ch->delay--;
    return r;
}

// What the mixer gets from a channel. Tonal voices: frequency and volume,
// silent unless gate() (the Arduboy holds a DC level instead). The noise
// voice: half the volume (its LFSR is its own).
static RAMFUNC(atmOut) void out(Osc *o, uint8_t n, const Chan &c) {
    if (n == 3) {
        o[3].vol = c.vol >> 1;
        return;
    }
    o[n].freq = c.freq;
    o[n].step = (uint32_t)c.freq * STEP;
    o[n].vol = gate(c, n) ? c.vol : 0;
}

// Every channel stopped: restart at the GOTO_ADV tracks, or end. Returns
// true when the song ended.
static RAMFUNC(atmEnd) bool endOrRestart(Score &s) {
    uint8_t repeat = 0;
    for (int j = 0; j < 4; j++) repeat = (uint8_t)(repeat + s.ch[j].repeatPoint);
    if (repeat) {
        for (int k = 0; k < 4; k++) {
            uint8_t t = s.ch[k].repeatPoint;
            s.ch[k].ptr = trackPtr(s, t < s.trackCount ? t : 0);
            s.ch[k].delay = 0;
        }
        s.active = 0x0F;
        return false;
    }
    s.ended = true;
    return true;
}

void scoreTick(Score &s, Osc *o) {
    if (s.ended) return;
    for (uint8_t n = 0; n < 4; n++) {
        if (step(s, &s.ch[n], o) & STOPPED) s.active &= (uint8_t)~(1 << n);
        out(o, n, s.ch[n]);
        if (!(s.active & 0x0F) && endOrRestart(s)) {
            for (int k = 0; k < 4; k++) o[k].vol = 0;
            return;
        }
    }
}

}  // namespace atm

using namespace atm;

// ---- The synth ---------------------------------------------------------------
// Shared with the interrupts: the score, the effect's channel, the voices.

static Score theScore;
static Chan sfxV;
static Osc osc[4];
static volatile uint8_t sfxOsc = 0xFF;      // the channel an effect holds
static volatile uint8_t sfxMask = 0;
static volatile uint8_t muteMask = 0;
static volatile bool scorePaused = false;
static volatile bool running = false;       // the playroutine is being ticked (a song or an effect)
static volatile bool loaded = false;        // a song is loaded and has not ended
static volatile uint32_t tickCount = 0;
static uint16_t tickAcc = 0;                // samples (or ms) towards the next tick, x tempo
static uint8_t driveQ4 = 24;
static uint16_t centre = 0;                 // the output's resting level, x4 (0 ... 512)
static uint16_t quietRun = 0;               // samples the mix has been silent
// The resting level is half way only while there is sound to swing around
// it: a quarter of a second of silence (a pause, a stop, a long rest) and it
// falls to 0, the pin low. A magnetic buzzer behind a transistor (Rev1)
// would otherwise carry a steady current; a piezo (Rev0) does not mind.
static const uint16_t QUIET = RATE / 4;
static volatile uint16_t peakV = 0;
static uint8_t eng = SYNTH;
static bool hwUp = false;

// One tick of the score and the effect: ATM_playroutine().
static RAMFUNC(atmTick) void tick() {
    for (uint8_t n = 0; n < 4; n++) {
        uint8_t bit = (uint8_t)(1 << n);
        if (scorePaused || !loaded) {
            if (!(sfxMask & bit)) osc[n].vol = 0;
            continue;
        }
        if (step(theScore, &theScore.ch[n], osc) & STOPPED) theScore.active &= (uint8_t)~bit;
        if (!(sfxMask & bit) && !(muteMask & bit)) out(osc, n, theScore.ch[n]);
        if (!(theScore.active & 0x0F)) {
            uint8_t repeat = 0;
            for (int j = 0; j < 4; j++) repeat = (uint8_t)(repeat + theScore.ch[j].repeatPoint);
            if (repeat) {
                endOrRestart(theScore);
                muteMask = 0;                   // as on the Arduboy: a restart clears the mutes
            } else if (sfxOsc >= 4) {
                theScore.ended = true;
                loaded = false;
                running = false;
                for (int k = 0; k < 4; k++) osc[k].vol = 0;
                return;
            }
        }
    }
    uint8_t n = sfxOsc;
    if (n < 4) {
        if (step(theScore, &sfxV, osc) & STOPPED) {
            sfxOsc = 0xFF;
            sfxMask = 0;
            if (!loaded) running = false;
        }
        if (!(muteMask & (1 << n))) out(osc, n, sfxV);
    }
    if (loaded && !scorePaused) tickCount++;
}

// One sample of the mix (ATMLIB_CONSTRUCT_ISR's), ticking the score when due.
static RAMFUNC(atmSample) uint8_t sample() {
    if (running) {
        if (tickAcc >= RATE) {
            tickAcc -= RATE;
            tick();
        }
        tickAcc += theScore.tickRate ? theScore.tickRate : 1;
    }
    if (osc[0].vol | osc[1].vol | osc[2].vol | osc[3].vol) quietRun = 0;
    else if (quietRun < QUIET) quietRun++;
    if (running && quietRun < QUIET) centre = centre < 510 ? centre + 2 : 512;    // up in 16 ms
    else if (centre) centre--;                                                    // down in 33 ms

    int s = 0;
    // CH0 pulse: low for the top quarter of the cycle
    osc[0].phase += osc[0].step;
    s += (osc[0].phase >> 24) >= 0xC0 ? -(int)osc[0].vol : (int)osc[0].vol;
    // CH1 square
    osc[1].phase += osc[1].step;
    s += (osc[1].phase & 0x80000000u) ? -(int)osc[1].vol : (int)osc[1].vol;
    // CH2 saw: (phase - 128) x vol / 256
    osc[2].phase += osc[2].step;
    s += ((int)(int8_t)((osc[2].phase >> 24) - 128) * (int)osc[2].vol) >> 8;
    // CH3 noise: the 16-bit LFSR in freq, a step every sample
    uint16_t x = (uint16_t)(osc[3].freq << 1);
    if (x & 0x8000) x ^= 1;
    if (x & 0x4000) x ^= 1;
    osc[3].freq = x;
    s += (x & 0x8000) ? -(int)osc[3].vol : (int)osc[3].vol;

    uint16_t a = (uint16_t)(s < 0 ? -s : s);
    if (a > peakV) peakV = a;
    int v = (centre >> 2) + ((s * driveQ4) >> 4);
    return (uint8_t)(v < 0 ? 0 : v > 255 ? 255 : v);
}

uint8_t ATMsynth::render() { return sample(); }

// ---- The hardware --------------------------------------------------------------
// Nothing here names a pin or a timer. The board package says where the
// buzzer is: its variant's PIN_BUZZER, looked up in the variant's own
// PinMap_TIM (the table analogWrite() uses), gives the timer, the channel
// and the remap; PIN_LED is the LED. So one source builds for every CHGame
// the installed board package knows, and each binary drives only its own
// board's buzzer and LED:
//
//   Rev0   PB10, TIM1 channel 2 (partial remap 1): a piezo; LED PB9
//   Rev1   PA1,  TIM2 channel 2 (remap 000): a MOSFET under a magnetic
//          buzzer; LED PB10 (PB9 is the backlight, CHGfx's: never touched)
//
// TIM1 and TIM2 are advanced timers on the CH32X035 (a repetition counter,
// MOE): an 8-bit PWM at 187.5 kHz, an update every 12th period. A timer
// without a repetition counter (read back as 0) gets the fallback: one
// period a sample, 3,072 counts, the PWM at the sample rate.
#if ATM_DEVICE
#include <PeripheralPins.h>
#include <pinmap.h>

#if !defined(PIN_BUZZER) || !defined(PIN_LED)
#error "CHJukebox: the selected board's variant names no PIN_BUZZER / PIN_LED"
#endif

static TIM_TypeDef *tim;                    // the buzzer's timer, from the variant
static volatile uint16_t *cvr;              // its channel's compare register
static volatile uint16_t *chctlr;           // the mode register holding that channel
static uint8_t chShift;                     // the channel's byte in it (0 or 8)
static uint16_t ccEnable;                   // its CCER enable bit
static IRQn_Type irq;
static uint8_t perSample = 1;               // compare counts a sample unit (12 without a repetition counter)
static bool hwOk = false;

static const uint8_t OC_PWM1 = 0x68, OC_LOW = 0x48;     // OCxM PWM mode 1 / forced low, with OCxPE

static void ocMode(uint8_t m) { *chctlr = (uint16_t)((*chctlr & ~(0xFFu << chShift)) | ((uint32_t)m << chShift)); }

// Where the variant says the buzzer is. false: no timer channel for it (or
// a complementary one, whose idle level would be high): stay silent.
static bool resolve() {
    PinName pin = digitalPinToPinName(PIN_BUZZER);
    tim = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
    uint32_t fn = pinmap_function(pin, PinMap_TIM);
    if (!tim || fn == (uint32_t)NC || CH_PIN_INVERTED(fn)) return false;
    uint8_t ch = (uint8_t)CH_PIN_CHANNEL(fn);
    if (ch < 1 || ch > 4) return false;
    cvr = &tim->CH1CVR + 2 * (ch - 1);      // CHxCVR, a word apart
    chctlr = ch <= 2 ? &tim->CHCTLR1 : &tim->CHCTLR2;
    chShift = (uint8_t)((ch - 1) % 2 * 8);
    ccEnable = (uint16_t)(1u << 4 * (ch - 1));
    if (tim == TIM1) {
        RCC->APB2PCENR |= RCC_APB2Periph_TIM1;
        irq = TIM1_UP_IRQn;
    } else if (tim == TIM2) {
        RCC->APB1PCENR |= RCC_APB1Periph_TIM2;
        irq = TIM2_UP_IRQn;
    } else if (tim == TIM3) {
        RCC->APB1PCENR |= RCC_APB1Periph_TIM3;
        irq = TIM3_IRQn;
    } else return false;
    RCC->APB2PCENR |= RCC_APB2Periph_AFIO;
    return true;
}

// The timer's common setup, its channel driving low throughout.
static void timerReset() {
    NVIC_DisableIRQ(irq);
    tim->CTLR1 = 0;
    tim->CTLR2 = 0;
    tim->SMCFGR = 0;
    tim->DMAINTENR = 0;
    ocMode(OC_LOW);
    *cvr = 0;
    tim->CNT = 0;
    tim->BDTR = 0x8000;                     // MOE (an advanced timer's outputs; ignored by a general one)
    tim->CCER = ccEnable;                   // this channel alone
}

static volatile uint8_t nextOut = 0;

// SYNTH: the sample written at each update; the compare is preloaded, so it
// lands on the next update, one sample later, every time.
static void hwSynth() {
    timerReset();
    tim->PSC = 0;                           // 48 MHz
    tim->RPTCR = 11;
    perSample = tim->RPTCR == 11 ? 1 : 12;  // a repetition counter, or the fallback
    tim->ATRLR = perSample == 1 ? 255 : 3071;
    tim->SWEVGR = 1;                        // load ARR, the compare and the repetition counter
    tim->INTFR = 0;
    ocMode(OC_PWM1);
    tim->DMAINTENR = 1;                     // the update interrupt
    NVIC_EnableIRQ(irq);
    tim->CTLR1 = 0x81;                      // ARPE + CEN
}

// BEEP: the CHGame library's tone path, a 1 MHz count and a period per note.
static uint16_t beepPeriod = 0;
static void hwBeep() {
    timerReset();
    tim->PSC = 47;                          // 1 MHz
    tim->RPTCR = 0;
    tim->ATRLR = 999;
    tim->SWEVGR = 1;
    tim->INTFR = 0;
    ocMode(OC_PWM1);
    tim->CTLR1 = 0x81;
    beepPeriod = 0;
}

static void hwInit() {
    hwUp = true;
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
    // The pin's latch low first, so it is never high on the way to the timer.
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);
    if (!(hwOk = resolve())) return;
    if (eng == BEEP) hwBeep(); else hwSynth();
    // Then the pin to the timer (its channel driving low already), with the
    // remap and mode the variant gives it.
    PinName pin = digitalPinToPinName(PIN_BUZZER);
    pin_function(pin, (int)pinmap_function(pin, PinMap_TIM));
}

static void onUpdate() {
    tim->INTFR = (uint16_t)~1u;
    *cvr = (uint16_t)(nextOut * perSample);
    nextOut = sample();
}

// Every timer the variant could name gets a handler; only the buzzer's has
// its interrupt enabled. (Peripherals "Game" compiles the core's own out.)
extern "C" void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
extern "C" void TIM1_UP_IRQHandler(void) { onUpdate(); }
extern "C" void TIM2_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
extern "C" void TIM2_UP_IRQHandler(void) { onUpdate(); }
extern "C" void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
extern "C" void TIM3_IRQHandler(void) { onUpdate(); }

// BEEP: every millisecond, tick the score when due and pick what the pin
// plays. The tonal voices take 6 ms turns (the library's arpeggio), a
// noise hit takes the pin for 12 ms of random pitches, a volume becomes a
// pulse width (1/16 at the quietest, 1/2 at 63). Notes under 400 Hz go up
// in octaves: the buzzer barely moves below that.
static uint8_t arpT, arpCh, noiseHold, lastNoise;
static uint16_t beepLfsr = 0xACE1;

static void beepTone(uint32_t hz, uint8_t vol) {
    if (!hz || !vol) {
        *cvr = 0;                           // low: silent
        return;
    }
    while (hz < 400) hz <<= 1;
    uint32_t period = (1000000u + hz / 2) / hz;
    if (period < 2) period = 2;
    if (vol > 63) vol = 63;
    tim->ATRLR = (uint16_t)(period - 1);
    *cvr = (uint16_t)(period * (vol + 9u) / 144u);
    if (period != beepPeriod && !beepPeriod) tim->SWEVGR = 1;
    beepPeriod = (uint16_t)period;
}

static bool beepAudible(uint8_t n) { return osc[n].vol >= 4 && osc[n].freq; }

extern "C" void osSystickHandler(void) {
    if (eng != BEEP || !hwOk) return;
    if (running) {
        tickAcc = (uint16_t)(tickAcc + (theScore.tickRate ? theScore.tickRate : 1));
        while (tickAcc >= 1000) {
            tickAcc -= 1000;
            tick();
        }
    }
    uint8_t nv = osc[3].vol;
    if (nv >= 6 && nv > lastNoise + 2) noiseHold = 12;
    lastNoise = nv;
    if (noiseHold) {
        noiseHold--;
        beepLfsr = (uint16_t)((beepLfsr >> 1) ^ (-(beepLfsr & 1u) & 0xB400u));
        beepTone(1800u + (beepLfsr & 4095u), (uint8_t)(nv * 2));
        return;
    }
    if (++arpT >= 6 || !beepAudible(arpCh)) {
        arpT = 0;
        for (int k = 0; k < 3; k++) {
            arpCh = (uint8_t)((arpCh + 1) % 3);
            if (beepAudible(arpCh)) break;
        }
    }
    if (beepAudible(arpCh)) beepTone((osc[arpCh].freq + 2u) / 4u, osc[arpCh].vol);
    else beepTone(0, 0);
    uint16_t a = osc[arpCh].vol;
    if (a > peakV) peakV = a;
}

static inline void irqOff() { __disable_irq(); }
static inline void irqOn() { __enable_irq(); }

#else

static void hwInit() { hwUp = true; }
static inline void irqOff() {}
static inline void irqOn() {}

#endif

// ---- The API -----------------------------------------------------------------------

void ATMsynth::play(const uint8_t *song) {
    irqOff();
    running = false;
    scoreLoad(theScore, song);
    memset(osc, 0, sizeof osc);
    osc[3].freq = 0x0001;                       // seed the LFSR
    sfxOsc = 0xFF;
    sfxMask = 0;
    muteMask = 0;
    scorePaused = false;
    tickCount = 0;
    tickAcc = eng == SYNTH ? RATE : 1000;       // the first tick at once
    loaded = true;
    running = true;
    irqOn();
    if (!hwUp) hwInit();
}

void ATMsynth::playPause() {
    if (scorePaused) resume();
    else pause();
}

void ATMsynth::pause() {
    if (scorePaused) return;
    irqOff();
    scorePaused = true;
    for (uint8_t n = 0; n < 4; n++)
        if (!(sfxMask & (1 << n))) osc[n].vol = 0;
    irqOn();
}

void ATMsynth::resume() {
    if (!scorePaused) return;
    scorePaused = false;
    if (loaded) running = true;
}

void ATMsynth::stop() {
    irqOff();
    running = false;
    loaded = false;
    memset(&theScore, 0, sizeof theScore);
    for (int n = 0; n < 4; n++) osc[n].vol = 0;
    sfxOsc = 0xFF;
    sfxMask = 0;
    muteMask = 0;
    scorePaused = false;
    irqOn();
}

void ATMsynth::muteChannel(uint8_t ch) {
    irqOff();
    muteMask |= (uint8_t)(1 << ch);
    if (!(sfxMask & (1 << ch))) osc[ch & 3].vol = 0;
    irqOn();
}

void ATMsynth::unMuteChannel(uint8_t ch) { muteMask &= (uint8_t)~(1 << ch); }

void ATMsynth::playSfx(const uint8_t *track, uint8_t ch) {
    if (ch > 3) return;
    irqOff();
    if (!running) {
        // No score: tick for the effect alone.
        if (!theScore.tickRate) theScore.tickRate = 25;
        tickAcc = eng == SYNTH ? RATE : 1000;
    }
    if (sfxOsc < 4 && sfxOsc != ch && !(muteMask & (1 << sfxOsc))) osc[sfxOsc].vol = 0;
    memset(&sfxV, 0, sizeof sfxV);
    sfxV.ptr = track;
    sfxOsc = ch;
    sfxMask = (uint8_t)(1 << ch);
    if (ch == 3) osc[3].freq = 0x0001;
    running = true;
    irqOn();
    if (!hwUp) hwInit();
}

void ATMsynth::setTempo(uint8_t t) { theScore.tickRate = t ? t : 1; }

uint8_t ATMsynth::check() {
    uint8_t v = theScore.cue;
    theScore.cue = 0;
    return v;
}

uint8_t ATMsynth::check(uint8_t id) { return theScore.cue == id; }

bool ATMsynth::playing() const { return loaded; }
bool ATMsynth::paused() const { return scorePaused; }
uint8_t ATMsynth::muted() const { return muteMask; }
uint32_t ATMsynth::ticks() const { return tickCount; }
const Score &ATMsynth::score() const { return theScore; }
uint8_t ATMsynth::sfxChannel() const { return sfxOsc; }

void ATMsynth::snapshot(Osc o[4]) const {
    irqOff();
    memcpy(o, osc, sizeof osc);
    irqOn();
}

uint16_t ATMsynth::peak() {
    uint16_t p = peakV;
    peakV = 0;
    return p;
}

void ATMsynth::setDrive(uint8_t q) { driveQ4 = q; }
uint8_t ATMsynth::drive() const { return driveQ4; }
uint8_t ATMsynth::engine() const { return eng; }

void ATMsynth::setEngine(uint8_t e) {
    if (e == eng) return;
    irqOff();
    eng = e;
    tickAcc = 0;
    irqOn();
#if ATM_DEVICE
    if (hwOk) {
        if (eng == BEEP) hwBeep(); else hwSynth();
    }
#endif
}

void ATMsynth::led(bool on) {
#if ATM_DEVICE
    if (hwUp) digitalWrite(PIN_LED, on ? HIGH : LOW);
#elif defined(CHSIM)
    digitalWrite(LED_BUILTIN, on ? HIGH : LOW);
#else
    (void)on;
#endif
}

// The simulator: render the samples up to `us` on the sketch's clock (its
// frames, so a lockstep run keeps the music in time with them). $ATM_WAV
// names a file to get the samples (8-bit unsigned, 15,625 Hz, mono).
#ifdef CHSIM
static FILE *wav;
static uint32_t wavBytes;
static void wavHeader() {
    uint8_t h[44];
    auto le32 = [&](int o, uint32_t v) { h[o] = (uint8_t)v; h[o + 1] = (uint8_t)(v >> 8); h[o + 2] = (uint8_t)(v >> 16); h[o + 3] = (uint8_t)(v >> 24); };
    memcpy(h, "RIFF", 4); le32(4, 36 + wavBytes); memcpy(h + 8, "WAVEfmt ", 8);
    le32(16, 16); le32(20, 1 | (1u << 16)); le32(24, RATE); le32(28, RATE); le32(32, 1 | (8u << 16));
    memcpy(h + 36, "data", 4); le32(40, wavBytes);
    fseek(wav, 0, SEEK_SET);
    fwrite(h, 1, 44, wav);
    fseek(wav, 0, SEEK_END);
    fflush(wav);
}
#endif

void ATMsynth::pump(uint32_t now) {
#ifdef CHSIM
    static bool started = false;
    static uint32_t last;
    static uint32_t frac;
    if (!started) {
        started = true;
        last = now;
        const char *path = getenv("ATM_WAV");
        if (path && *path && (wav = fopen(path, "wb")) != nullptr) wavHeader();
        return;
    }
    uint64_t t = (uint64_t)(now - last) * RATE + frac;
    last = now;
    uint32_t n = (uint32_t)(t / 1000000u);
    frac = (uint32_t)(t % 1000000u);
    for (uint32_t i = 0; i < n; i++) {
        uint8_t v = sample();
        if (wav) fputc(v, wav);
    }
    if (wav && n) {
        wavBytes += n;
        wavHeader();
    }
#else
    (void)now;
#endif
}
