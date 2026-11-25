"""atmgen: write ATMlib songs in Python, assemble them, prove they loop.

A song is four parts (one per channel), each a list of commands and waits
written in ticks. The assembler cuts every part into bars, gives each bar
that comes back a track of its own (called with GOTO, or REPEAT for a run
of the same bar), writes the rest inline, and loops the body the hop-free
way of ATMlib's README section 6: every channel's loop track ends with a
GOTO to itself and holds exactly the loop's ticks. Track 0 is a silent
dummy nobody calls (the README's track-0 trap), and an unused channel
points at it.

make_songs.py runs every song through tools/songs/atmrender.cpp (the port
itself, on the PC) and checks what ATMlib's README asks: no channel stops,
the body repeats tick for tick, the call stack stays under 7, no note
leaves C2..D7; then compiles the header it wrote against the real
ATMcmds.h and compares the bytes.

Notes are names ("C4", "F#5", "Bb3"; C2 = 1 ... D7 = 63, ATM_NOTE_A4 is 34
and sounds at 440 Hz on the CHGame) or numbers. Part.play() takes a line of
tokens, each a note, a rest or a command, with a default length:

    p.play("L4 D5 E5 F5 G5 | A5:8 - | !v30 Bb5:12 =:4")
      L4      default length 4 ticks from here on
      D5      note D5 for the default length; D5:8 for 8 ticks
      -  -:8  rest (note off)            =  =:8  hold the last note longer
      |       a bar line: checks the bar is full (bar length from the song)
      !v30 !s-2 !t-5 !T2  VOL, SL_VOL, SET_TRA, ADD_TRA
      !a37:21 !ao          ARP(0x37, 0x21), ARP_OFF
      !vib3:3 !vo !trem1:7 !tro   VIB, VIB_OFF, TREM, TREM_OFF
      !f-20 !fo            SL_FRQ (Hz per tick), SL_FRQ_OFF
      !g1 !g-2 !go         GLIS (up 1 tick a step / down 2), GLIS_OFF
      !cut3 !co !so        CUT(0x20 | 3), CUT_OFF, SL_VOL_OFF
"""
import re

# ---- Opcodes (ATMcmds.h) -----------------------------------------------------------

LETTER = {"C": 0, "D": 2, "E": 4, "F": 5, "G": 7, "A": 9, "B": 11}
NAMES = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]


def note(x):
    """A note name or number -> ATMlib's number (C2 = 1, D7 = 63; 0 = off)."""
    if isinstance(x, int):
        return x
    m = re.fullmatch(r"([A-Ga-g])([#b]*)(-?\d)", x.strip())
    if not m:
        raise ValueError(f"bad note {x!r}")
    n = (int(m.group(3)) - 2) * 12 + LETTER[m.group(1).upper()] + m.group(2).count("#") - m.group(2).count("b") + 1
    if not 1 <= n <= 63:
        raise ValueError(f"note {x!r} is out of C2..D7")
    return n


def macro_note(n):
    if n == 0:
        return "0x00"
    n0 = n - 1
    name = NAMES[n0 % 12]
    return f"ATM_NOTE_{name[0]}{2 + n0 // 12}{'_' if len(name) > 1 else ''}"


def s8(v):
    if not -128 <= v <= 255:
        raise ValueError(f"{v} does not fit a byte")
    return v & 255


def arg(v):
    return f"(uint8_t){v}" if v < 0 else f"{v}" if v < 10 else f"0x{v:02X}" if v > 31 else f"{v}"


class Cmd:
    """One command: its bytes and the ATM_* text that writes them."""
    __slots__ = ("b", "text", "kind")

    def __init__(self, b, text, kind="fx"):
        self.b = bytes(b)
        self.text = text
        self.kind = kind


def c_vol(v): return Cmd([0x40, s8(v)], f"ATM_VOL({v})")
def c_slvol(s): return Cmd([0x41, s8(s)], f"ATM_SL_VOL({arg(s)})")
def c_slvol_adv(s, t): return Cmd([0x42, s8(s), t], f"ATM_SL_VOL_ADV({arg(s)}, {t})")
def c_slvol_off(): return Cmd([0x43], "ATM_SL_VOL_OFF")
def c_slfrq(s): return Cmd([0x44, s8(s)], f"ATM_SL_FRQ({arg(s)})")
def c_slfrq_off(): return Cmd([0x46], "ATM_SL_FRQ_OFF")
def c_arp(a, t): return Cmd([0x47, a, t], f"ATM_ARP(0x{a:02X}, 0x{t:02X})")
def c_arp_off(): return Cmd([0x48], "ATM_ARP_OFF")
def c_noise(x): return Cmd([0x49, x], f"ATM_NOISE(0x{x:02X})")
def c_noise_off(): return Cmd([0x4A], "ATM_NOISE_OFF")
def c_add_tra(v): return Cmd([0x4B, s8(v)], f"ATM_ADD_TRA({arg(v)})")
def c_set_tra(v): return Cmd([0x4C, s8(v)], f"ATM_SET_TRA({arg(v)})")
def c_trem(d, r): return Cmd([0x4E, d, r], f"ATM_TREM({d}, {r})")
def c_trem_off(): return Cmd([0x4F], "ATM_TREM_OFF")
def c_vib(d, r): return Cmd([0x50, d, r], f"ATM_VIB({d}, {r})")
def c_vib_off(): return Cmd([0x51], "ATM_VIB_OFF")
def c_glis(x): return Cmd([0x52, x], f"ATM_GLIS(0x{x:02X})")
def c_glis_off(): return Cmd([0x53], "ATM_GLIS_OFF")
def c_cut(x): return Cmd([0x54, x], f"ATM_CUT(0x{x:02X})")
def c_cut_off(): return Cmd([0x55], "ATM_CUT_OFF")
def c_cue(v): return Cmd([0x57, v], f"ATM_CUE({v})")
def c_set_tempo(v): return Cmd([0x9D, v], f"ATM_SET_TEMPO({v})")
def c_add_tempo(v): return Cmd([0x9C, s8(v)], f"ATM_ADD_TEMPO({arg(v)})")
def c_note(n): return Cmd([n], macro_note(n), "note")
def c_delay(t):
    assert 1 <= t <= 64, t
    return Cmd([0x9F + t], f"ATM_DELAY({t})", "delay")
def c_goto(t): return Cmd([0xFC, t], f"ATM_GOTO({t})", "call")
def c_repeat(n, t): return Cmd([0xFD, n, t], f"ATM_REPEAT({n}, {t})", "call")
C_RETURN = Cmd([0xFE], "ATM_RETURN", "end")
C_STOP = Cmd([0x9F], "ATM_STOP_CHAN", "end")


# ---- Parts ---------------------------------------------------------------------------

class Part:
    """One channel: commands and waits, in ticks."""

    def __init__(self, song, ch):
        self.song = song
        self.ch = ch
        self.items = []         # (tick, Cmd) or (tick, ticks) for a wait
        self.t = 0
        self.default = 4
        self.last = 0

    # raw
    def cmd(self, c):
        self.items.append((self.t, c))
        return self

    def wait(self, ticks):
        if ticks < 0:
            raise ValueError("negative wait")
        if ticks:
            self.items.append((self.t, ticks))
            self.t += ticks
        return self

    def note(self, n, ticks=None):
        n = note(n)
        self.last = n
        self.cmd(c_note(n))
        return self.wait(self.default if ticks is None else ticks)

    def rest(self, ticks=None):
        self.last = 0
        self.cmd(c_note(0))
        return self.wait(self.default if ticks is None else ticks)

    # effects
    def vol(self, v): return self.cmd(c_vol(v))
    def slide(self, s): return self.cmd(c_slvol(s))
    def slide_adv(self, s, t): return self.cmd(c_slvol_adv(s, t))
    def slide_off(self): return self.cmd(c_slvol_off())
    def fslide(self, s): return self.cmd(c_slfrq(s))
    def fslide_off(self): return self.cmd(c_slfrq_off())
    def arp(self, a, t=0x20): return self.cmd(c_arp(a, t))
    def arp_off(self): return self.cmd(c_arp_off())
    def tra(self, v): return self.cmd(c_set_tra(v))
    def add_tra(self, v): return self.cmd(c_add_tra(v))
    def vib(self, d, r): return self.cmd(c_vib(d, r))
    def vib_off(self): return self.cmd(c_vib_off())
    def trem(self, d, r): return self.cmd(c_trem(d, r))
    def trem_off(self): return self.cmd(c_trem_off())
    def glis(self, x): return self.cmd(c_glis(x))
    def glis_off(self): return self.cmd(c_glis_off())
    def cut(self, x): return self.cmd(c_cut(x))
    def cut_off(self): return self.cmd(c_cut_off())
    def noise(self, x): return self.cmd(c_noise(x))
    def noise_off(self): return self.cmd(c_noise_off())
    def cue(self, v): return self.cmd(c_cue(v))
    def tempo(self, v): return self.cmd(c_set_tempo(v))
    def add_tempo(self, v): return self.cmd(c_add_tempo(v))

    def play(self, text):
        bar_start = self.t
        for tok in text.split():
            if tok == "|":
                if (self.t - bar_start) % self.song.bar:
                    raise ValueError(f"ch{self.ch}: bar of {self.t - bar_start} ticks at tick {self.t}: {text!r}")
                bar_start = self.t
                continue
            if tok[0] == "L":
                self.default = int(tok[1:])
                continue
            if tok[0] == "!":
                self._command(tok[1:])
                continue
            name, _, ln = tok.partition(":")
            ticks = int(ln) if ln else self.default
            if name == "-":
                self.rest(ticks)
            elif name == "=":
                self.wait(ticks)
            elif name == "~":                 # a wait without a new note-off (a rest already sounding)
                self.wait(ticks)
            else:
                self.note(name, ticks)
        return self

    def _command(self, c):
        def nums(s):
            return [int(x, 16) if x.startswith("0x") else int(x) for x in s.split(":")]
        if c.startswith("vib"):
            d, r = nums(c[3:]); self.vib(d, r)
        elif c == "vo":
            self.vib_off()
        elif c.startswith("trem"):
            d, r = nums(c[4:]); self.trem(d, r)
        elif c == "tro":
            self.trem_off()
        elif c.startswith("cut"):
            self.cut(0x20 | int(c[3:]))
        elif c == "co":
            self.cut_off()
        elif c == "so":
            self.slide_off()
        elif c == "ao":
            self.arp_off()
        elif c == "fo":
            self.fslide_off()
        elif c == "go":
            self.glis_off()
        elif c[0] == "a":
            a, t = c[1:].split(":")
            self.arp(int(a, 16), int(t, 16))
        elif c[0] == "v":
            self.vol(int(c[1:]))
        elif c[0] == "s":
            self.slide(int(c[1:]))
        elif c[0] == "t":
            self.tra(int(c[1:]))
        elif c[0] == "T":
            self.add_tra(int(c[1:]))
        elif c[0] == "f":
            self.fslide(int(c[1:]))
        elif c[0] == "g":
            g = int(c[1:])
            self.glis(g if g > 0 else 0x80 | -g)
        else:
            raise ValueError(f"unknown command !{c}")

    def to(self, tick):
        """Wait (without a note-off) until an absolute tick."""
        return self.wait(tick - self.t)


# ---- The song --------------------------------------------------------------------------

class Song:
    def __init__(self, ident, title, style, key, tempo, beat=16, beats=4, prompt=""):
        self.ident = ident          # the C name
        self.title = title
        self.style = style
        self.key = key
        self.tempo = tempo          # ticks a second
        self.beat = beat            # ticks a beat
        self.beats = beats          # beats a bar
        self.bar = beat * beats
        self.prompt = prompt
        self.parts = [Part(self, n) for n in range(4)]
        self.loop_at = 0            # where the body starts (ticks)
        self.length = None          # the end of the body
        self.sections = []          # (tick, name): ATM_CUE(i + 1) at each
        self.used = [True, True, True, True]

    def __getitem__(self, n):
        return self.parts[n]

    def section(self, tick, name):
        self.sections.append((tick, name))

    def bpm(self):
        return self.tempo * 60 / self.beat

    # ---- assembly ----
    def _bars(self, part):
        """Cut a part into bars: lists of Cmd. Time is rebuilt from the
        commands' ticks, so waits merge, and delays break at bar lines."""
        cmds = {}
        for t, x in part.items:
            if isinstance(x, Cmd):
                if t >= self.length:
                    raise ValueError(f"ch{part.ch}: {x.text} at tick {t}, at or past the end ({self.length})")
                cmds.setdefault(t, []).append(x)
        points = sorted(set(cmds) | set(range(0, self.length, self.bar)))
        bars = [[] for _ in range(self.length // self.bar)]
        for i, t in enumerate(points):
            bar = bars[t // self.bar]
            bar.extend(cmds.get(t, []))
            d = (points[i + 1] if i + 1 < len(points) else self.length) - t
            while d:
                c = min(d, 64)
                bar.append(c_delay(c))
                d -= c
        return bars

    def assemble(self):
        n_bars = self.length // self.bar
        intro_bars = self.loop_at // self.bar
        if self.loop_at % self.bar or self.length % self.bar:
            raise ValueError("the loop must start and end on a bar line")
        # Sections become cues on the first used channel.
        first = next(i for i in range(4) if self.used[i])
        for i, (t, _) in enumerate(self.sections):
            self.parts[first].items.append((t, c_cue(i + 1)))
        for p in self.parts:
            # Commands at a tick before the wait that starts there; stable otherwise.
            p.items.sort(key=lambda it: (it[0], not isinstance(it[1], Cmd)))
        # Tempo on the first channel at the start of the body (and of the intro).
        self.parts[first].items.insert(0, (0, c_set_tempo(self.tempo)))
        if self.loop_at:
            idx = next(i for i, (t, _) in enumerate(self.parts[first].items) if t >= self.loop_at)
            self.parts[first].items.insert(idx, (self.loop_at, c_set_tempo(self.tempo)))

        per_ch = []
        for p in self.parts:
            if not self.used[p.ch]:
                per_ch.append(None)
                continue
            if p.t != self.length:
                raise ValueError(f"ch{p.ch} is {p.t} ticks, the song {self.length}")
            per_ch.append(self._bars(p))

        # Count uses of each distinct bar, per run (a run of equal bars is one REPEAT).
        def runs(bars, lo, hi):
            out = []
            i = lo
            while i < hi:
                j = i
                while j + 1 < hi and bars[j + 1] and [c.b for c in bars[j + 1]] == [c.b for c in bars[i]]:
                    j += 1
                out.append((i, j - i + 1))
                i = j + 1
            return out

        key = lambda bar: b"".join(c.b for c in bar)
        inline = {}                                 # bytes if every use is written out
        called = {}                                 # bytes of the calls if it is a track
        layout = []
        for ch, bars in enumerate(per_ch):
            if bars is None:
                layout.append(None)
                continue
            segs = [runs(bars, 0, intro_bars), runs(bars, intro_bars, n_bars)]
            for seg in segs:
                for i, n in seg:
                    k = key(bars[i])
                    inline[k] = inline.get(k, 0) + n * len(k)
                    called[k] = called.get(k, 0) + (3 if n > 1 else 2)
            layout.append(segs)
        # A track costs its bytes, its RETURN and 2 in the table (README 7.1).
        share = {k for k in inline if len(k) + 3 + called[k] < inline[k]}

        tracks = [[C_STOP]]                         # track 0: the silent dummy
        names = ["silent (unused channels; never called)"]
        shared = {}
        entries = [0, 0, 0, 0]

        def bar_track(k, bar, label):
            if k not in shared:
                shared[k] = len(tracks)
                tracks.append(list(bar) + [C_RETURN])
                names.append(label)
            return shared[k]

        def body(bars, seg, label):
            out = []
            for i, n in seg:
                k = key(bars[i])
                if k in share:
                    t = bar_track(k, bars[i], f"{label} bar {i + 1}")
                    out.append(c_repeat(n - 1, t) if n > 1 else c_goto(t))
                else:
                    out.extend(bars[i] * n)
            return out

        for ch, segs in enumerate(layout):
            if segs is None:
                continue
            bars = per_ch[ch]
            label = f"ch{ch}"
            intro, loop = segs
            if intro:
                e = len(tracks)
                tracks.append(None)
                names.append(f"{label} intro")
                lt = len(tracks)
                tracks.append(None)
                names.append(f"{label} loop")
                tracks[e] = body(bars, intro, label) + [c_goto(lt)]
                tracks[lt] = body(bars, loop, label) + [c_goto(lt)]
                entries[ch] = e
            else:
                e = len(tracks)
                tracks.append(None)
                names.append(f"{label} loop")
                tracks[e] = body(bars, loop, label) + [c_goto(e)]
                entries[ch] = e
        if len(tracks) > 255:
            raise ValueError("too many tracks")
        self.tracks, self.track_names, self.entries = tracks, names, entries
        data = bytearray([len(tracks)])
        off = 0
        for t in tracks:
            data += bytes([off & 255, off >> 8])
            off += sum(len(c.b) for c in t)
        data += bytes(entries)
        for t in tracks:
            for c in t:
                data += c.b
        self.data = bytes(data)
        return self.data

    def header(self):
        """The song as an ATMlib song.h: ATM_* commands, the header in hex."""
        d = self.data
        out = []
        w = out.append
        w(f"// \"{self.title}\" - {self.style}, {self.key}. {len(d)} bytes.")
        w(f"// Written by tools/songs/make_songs.py ({self.ident}.py): do not edit by hand.")
        w(f"// Tempo {self.tempo} ({self.bpm():.0f} BPM: {self.beat} ticks a beat, {self.beats} beats a bar).")
        loop = (self.length - self.loop_at) // self.bar
        intro = self.loop_at // self.bar
        w(f"// {intro} bars of intro, then {loop} bars that loop (every channel's loop track ends with")
        w("// ATM_GOTO(itself), so the loop has no seam). CH0 pulse, CH1 square, CH2 saw, CH3 noise.")
        if self.prompt:
            w("//")
            for line in self.prompt.strip().splitlines():
                w(("// " + line.strip()).rstrip())
        w("#pragma once")
        w("#include \"../atm/ATMcmds.h\"")
        w("")
        w(f"const uint8_t {self.ident}[] = {{")
        w(f"    0x{len(self.tracks):02X},  // tracks")
        off = 0
        for i, t in enumerate(self.tracks):
            w(f"    0x{off & 255:02X}, 0x{off >> 8:02X},  // track {i}: {self.track_names[i]}")
            off += sum(len(c.b) for c in t)
        for ch in range(4):
            w(f"    0x{self.entries[ch]:02X},  // CH{ch} entry")
        for i, t in enumerate(self.tracks):
            w(f"    // track {i}: {self.track_names[i]}")
            line = "   "
            for c in t:
                piece = " " + c.text + ","
                if len(line) + len(piece) > 100:
                    w(line)
                    line = "   "
                line += piece
            w(line)
        w("};")
        return "\n".join(out) + "\n"
