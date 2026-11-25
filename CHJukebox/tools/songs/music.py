"""Music helpers for CHJukebox's songs: scales, diatonic moves, chord
figures, a drum kit for the noise channel, and a two-voice counterpoint
check. Notes are ATMlib numbers (atmgen.note: C2 = 1)."""
from atmgen import note

NAMES = ["C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"]
MODES = {
    "major": [0, 2, 4, 5, 7, 9, 11],
    "minor": [0, 2, 3, 5, 7, 8, 10],
    "harmonic": [0, 2, 3, 5, 7, 8, 11],
    "dorian": [0, 2, 3, 5, 7, 9, 10],
    "mixolydian": [0, 2, 4, 5, 7, 9, 10],
}


def pc(n):
    return (note(n) - 1) % 12


def name(n):
    n = note(n) - 1
    return f"{NAMES[n % 12]}{2 + n // 12}"


class Scale:
    def __init__(self, tonic, mode="major"):
        self.pcs = [(pc(tonic + "4") + i) % 12 for i in MODES[mode]]

    def with_(self, *alter):
        """A copy with notes altered by letter: with_('C#') turns C into C#, with_('B') Bb into B."""
        s = Scale.__new__(Scale)
        s.pcs = list(self.pcs)
        for a in alter:
            p = pc(a + "4")
            nat = pc(a[0] + "4")
            for cand in (nat, (nat + 1) % 12, (nat - 1) % 12):
                if cand in s.pcs:
                    s.pcs[s.pcs.index(cand)] = p
                    break
        return s

    def notes(self):
        return [m for m in range(1, 64) if (m - 1) % 12 in self.pcs]

    def step(self, n, k):
        """Move note n by k scale steps (n snaps down to the scale first)."""
        n = note(n)
        while (n - 1) % 12 not in self.pcs:
            n -= 1
        ns = self.notes()
        return ns[ns.index(n) + k]

    def run(self, n, steps):
        return [self.step(n, k) for k in steps]


def seq(part, notes, ticks):
    """Play notes (names, numbers, '-' rests, or (note, ticks) pairs)."""
    for x in notes:
        if isinstance(x, tuple):
            x, t = x
        else:
            t = ticks
        if x == "-" or x == 0:
            part.rest(t)
        elif x == "=":
            part.wait(t)
        else:
            part.note(x, t)


class Kit:
    """The noise channel as a drum kit. An instrument is (tag, volume,
    slide[, noise retrigger]): the tag is the note the hit is written
    with, which the noise voice ignores but the visualiser reads (kick,
    snare, hat ... each a lane). A hit is NOTE(tag), VOL, and SL_VOL /
    NOISE when they change; state is tracked within a bar only, so every
    bar stands alone and equal bars share a track."""

    def __init__(self, part, inst):
        self.p = part
        self.inst = inst
        self.reset()

    def reset(self):
        self.slide = None
        self.noise = 0

    def hit(self, name, ticks, vol=None):
        tag, v, s, *rest = self.inst[name]
        nz = rest[0] if rest else 0
        if self.p.t % self.p.song.bar == 0:
            self.reset()
        p = self.p
        if nz != self.noise:
            if nz:
                p.noise(nz)
            else:
                p.noise_off()
            self.noise = nz
        if s != self.slide:
            p.slide(s)
            self.slide = s
        p.note(tag, 0)
        p.vol(v if vol is None else vol)
        p.wait(ticks)

    def pattern(self, text, step):
        """One character a step: an instrument's letter, or '.' to wait."""
        for c in text.replace(" ", "").replace("|", ""):
            if c == ".":
                if self.p.t % self.p.song.bar == 0:
                    self.reset()
                self.p.wait(step)
            else:
                self.hit(c, step)


def counterpoint(upper, lower, step, strong, label=""):
    """Print the dissonances between two parts on strong positions.
    upper/lower: atmgen Parts; step: the grid in ticks; strong: the grid of
    the strong positions (e.g. 8 for every eighth)."""
    def sounding(part):
        out = {}
        cur, tra = 0, 0
        for t, x in part.items:
            if hasattr(x, "kind") and x.kind == "note":
                cur = x.b[0]
                out[t] = cur
        return out
    u, l = sounding(upper), sounding(lower)
    end = min(upper.t, lower.t)
    cu = cl = 0
    bad = []
    for t in range(0, end, step):
        cu = u.get(t, cu) if t in u else cu
        cl = l.get(t, cl) if t in l else cl
        if t % strong == 0 and cu and cl:
            iv = (cu - cl) % 12
            if iv in (1, 2, 6, 10, 11):
                bar = t // upper.song.bar + 1
                bad.append(f"bar {bar} tick {t % upper.song.bar}: {name(cu)} over {name(cl)}")
    if bad:
        print(f"  {label} {len(bad)} strong dissonances:")
        for b in bad:
            print("   ", b)
    return bad
