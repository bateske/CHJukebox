"""CLOCKWORK INVENTION: a two-part invention in the style of Bach."""
from atmgen import Song
from music import Scale, seq, counterpoint, Kit

PROMPT = """
The brief: "A Bach-style two-part invention in D minor at 112 BPM for a
clockwork harpsichord. Strict two-voice counterpoint: a one-bar subject in
running sixteenths (a scale that climbs to the leading tone and snaps back
down) stated alone in the right hand, answered at the octave in the left
while the right hand plays a countersubject in steady eighths. A descending
circle-of-fifths episode (Dm Gm C F Bb E-dim A Dm) built from one figure
moved down a step every bar, a middle entry in the relative major (F) in
both voices, a second episode with the roles swapped (the left hand runs,
the right hand sings a falling scale), then the final entries, the subject
an octave higher at the climax, a dominant pedal on A where both hands run
in tenths, and a cadence onto a D major chord (a Picardy third). Every note
a pluck that dies away. No drums: a quiet clock ticks the beats."
CH0 pulse: the right hand. CH1 square: the left hand. CH2 saw: the
continuo, only for the pedal and the cadence. CH3 noise: the clock.
"""

DM = Scale("D", "minor")
HM = DM.with_("C#")                 # harmonic minor (the A chord)
AM = DM.with_("C#", "B")            # over A: B natural on the way down
FM = Scale("F", "major")

SUBJECT = ["-", "D5", "E5", "F5", "G5", "A5", "Bb5", "G5", "E5", "C#6", "D6", "A5", "F5", "D5", "E5", "C#5"]
COUNTER = [("D5", 8), ("A5", 8), ("Bb5", 8), ("G5", 8), ("A5", 8), ("F5", 8), ("D5", 8), ("E5", 8)]


def figure(scale, root):
    """The episode figure over one chord: 5 4 3 2 | 3 1 5 3 in sixteenths."""
    return [scale.step(root, k) for k in (4, 3, 2, 1, 2, 0, 4, 2)]


def build():
    s = Song("invention", "CLOCKWORK INVENTION", "BAROQUE", "D MINOR", tempo=30, beat=16, beats=4, prompt=PROMPT)
    s.blurb = "TWO-PART INVENTION FOR A CLOCKWORK HARPSICHORD"
    U, L, B, D = s[0], s[1], s[2], s[3]
    bar = s.bar

    # The pluck: loud at the note, dying away.
    for p, v in ((U, 34), (L, 30)):
        p.vol(v).slide(-2)

    # ---- Exposition (bars 1-2) ----
    s.section(0, "EXPOSITION")
    seq(U, SUBJECT, 4)
    L.rest(bar)
    seq(U, COUNTER, None)
    seq(L, [x if x == "-" else _n(x) - 12 for x in SUBJECT], 4)

    # ---- Episode 1 (bars 3-6): the circle of fifths ----
    s.section(2 * bar, "EPISODE")
    chords = [("D5", DM, "D3", "minor"), ("G5", DM, "G3", "minor"), ("C5", DM, "C3", "major"), ("F5", DM, "F3", "major"),
              ("Bb4", DM, "Bb2", "major"), ("E5", DM, "E3", "dim"), ("A4", AM, "A2", "major"), ("D5", DM, "D3", "minor")]
    for i, (root, sc, bass, q) in enumerate(chords):
        seq(U, figure(sc, root), 4)
        b = _n(bass)
        third = b + (3 if q in ("minor", "dim") else 4)
        fifth = b + (6 if q == "dim" else 7)
        if (i // 2) % 2 == 0:
            seq(L, [b, third, fifth, third], 8)           # root 3 5 3
        else:
            seq(L, [b, fifth, b + 12, fifth], 8)          # root 5 8 5

    # ---- Middle entries in F major (bars 7-8) ----
    s.section(6 * bar, "F MAJOR")
    subj_f = ["-", "F5", "G5", "A5", "Bb5", "C6", "D6", "Bb5", "G5", "E6", "F6", "C6", "A5", "F5", "G5", "E5"]
    counter_f = [("F5", 8), ("C6", 8), ("D6", 8), ("Bb5", 8), ("C6", 8), ("A5", 8), ("F5", 8), ("G5", 8)]
    seq(U, subj_f, 4)
    seq(L, [(_n(x) - 12, t) for x, t in counter_f], None)
    seq(U, counter_f, None)
    seq(L, [x if x == "-" else _n(x) - 12 for x in subj_f], 4)

    # ---- Episode 2 (bars 9-10): the left hand runs, the right hand falls ----
    s.section(8 * bar, "EPISODE II")
    for root, sc in (("Bb3", DM), ("E4", DM), ("A3", AM), ("D4", DM)):
        seq(L, figure(sc, root), 4)
    seq(U, ["D6", "C6", "Bb5", "A5", "G5", "F5", "E5", "D5", "C#5", "E5", "A5", "G5", "F5", "E5", "D5", "C#5"], 8)

    # ---- Final entries (bars 11-12): the subject, then an octave up ----
    s.section(10 * bar, "RETURN")
    seq(U, SUBJECT, 4)
    seq(L, [(_n(x) - 12, t) for x, t in COUNTER], None)
    seq(U, [x if x == "-" else _n(x) + 12 for x in SUBJECT], 4)
    seq(L, COUNTER, None)

    # ---- Dominant pedal (bars 13-14): tenths over a held A ----
    s.section(12 * bar, "PEDAL")
    up = ["E5", "F5", "G5", "A5", "Bb5", "C#6", "D6", "E6", "F6", "E6", "D6", "C#6", "D6", "Bb5", "G5", "E5",
          "C#5", "E5", "G5", "Bb5", "A5", "C#6", "E6", "G6", "F6", "E6", "D6", "C#6", "D6", "C#6", "Bb5", "G5"]
    seq(U, up, 4)
    seq(L, [HM.step(x, -9) for x in up], 4)          # a tenth below, in the harmonic minor

    # ---- Cadence (bars 15-16): iv6 - V7 - I (major) ----
    s.section(14 * bar, "CADENCE")
    seq(U, ["Bb5", "A5", "G5", "F5", "G5", "D5", "Bb5", "G5", ("C#6", 16), ("E6", 8), ("C#6", 8)], 4)
    seq(L, [("Bb3", 8), ("D4", 8), ("G3", 8), ("Bb3", 8), ("A3", 8), ("G3", 8), ("A3", 8), ("A2", 8)], None)
    seq(U, [("D6", 8), ("A5", 4), ("F#5", 4), ("D5", 16), ("-", 32)], None)
    seq(L, [("D3", 8), ("A3", 4), ("F#3", 4), ("D3", 16), ("-", 32)], None)

    # ---- The continuo: silent until the last entry, then pedal and cadence ----
    B.rest(10 * bar)
    B.vol(22).slide(0)
    B.note("D3", bar).note("D3", bar)
    B.vol(30)
    B.note("A2", 2 * bar)
    B.vol(26).slide(-1)
    B.note("Bb2", bar // 2).note("A2", bar // 2)
    B.note("D3", bar // 2).rest(bar // 2)

    # ---- The clock: a tock on the bar, ticks on the beats ----
    kit = Kit(D, {"t": (25, 12, -6), "T": (13, 20, -5)})
    for _ in range(16):
        kit.pattern("T" + "." * 15 + "t" + "." * 15 + "t" + "." * 15 + "t" + "." * 15, 1)

    s.length = 16 * bar
    s.loop_at = 0
    if __name__ == "__main__":
        counterpoint(U, L, 4, 16, "upper/lower")
    return s


def _n(x):
    from atmgen import note
    return note(x)


if __name__ == "__main__":
    build()
