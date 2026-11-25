"""THE BRASS KETTLE: an Irish double jig."""
from atmgen import Song, note
from music import seq, Kit

PROMPT = """
The brief: "A rollicking Irish double jig in 6/8 at 117 dotted-quarter
beats a minute, in D major with a mixolydian turn (C natural) in the high
part. A fiddle tune of two 8-bar parts played AABB, the way a session plays
it: the lilt of a jig is in its 'D-D-D' figures and its runs; the second
time through each part the fiddler ornaments it, with cuts (a flick of a
higher note before a repeated one), a roll on the long notes and slides up
into the high notes. Under it a drone of open fifths like uilleann pipes,
swelling slowly; a bouzouki-style bass that bounces root and fifth on the
beat; and a bodhran: a deep stroke on each beat, two lighter ones after it,
and a rolled fill at the end of every part. Two bars of drone and drum to
count it in."
CH0 pulse: the fiddle. CH1 square: the drone. CH2 saw: the bass.
CH3 noise: the bodhran.
"""

E = 6           # an eighth note
Q = 12
DQ = 18

# The tune, in eighths (6 ticks); ("X", n) holds n ticks.
A_PART = [
    ["D5", "E5", "F#5", "A5", "F#5", "D5"],
    ["G5", "F#5", "E5", "F#5", "D5", "B4"],
    ["A4", "D5", "D5", "F#5", "D5", "D5"],
    ["E5", "F#5", "G5", ("A5", Q), "G5"],
    ["F#5", "A5", "A5", "B5", "A5", "A5"],
    ["D6", "C#6", "B5", "A5", "F#5", "D5"],
    ["G5", "B5", "G5", "F#5", "A5", "F#5"],
    ["E5", "D5", "C#5", ("D5", Q), "A4"],
]
B_PART = [
    ["D6", "A5", "F#5", "A5", "D6", "E6"],
    ["F#6", "E6", "D6", ("C6", Q), "A5"],
    ["B5", "G5", "G5", "D5", "G5", "G5"],
    ["A5", "B5", "C#6", "D6", "E6", "F#6"],
    ["G6", "F#6", "E6", "F#6", "D6", "A5"],
    ["E6", "C6", "A5", "G5", "A5", "C6"],
    ["B5", "A5", "G5", "F#5", "E5", "D5"],
    ["E5", "F#5", "E5", ("D5", Q), "A5"],
]
# Chords, one a beat (two a bar).
A_CHORDS = ["D D", "G B", "D D", "A A", "D D", "D A", "G D", "A D"]
B_CHORDS = ["D D", "C C", "G G", "A A", "D D", "C C", "G D", "A D"]
ROOT = {"D": "D3", "G": "G3", "A": "A3", "B": "B2", "C": "C3"}


DMAJ = {1, 2, 4, 6, 7, 9, 11}


def upper(n):
    """The scale note above n (D major): a cut's grace, a roll's upper note."""
    n = note(n)
    if (n - 1) % 12 == 0:               # C natural (the mixolydian bars): D above
        return n + 2
    return n + 1 if (n) % 12 in DMAJ else n + 2


def lower(n):
    n = note(n)
    if (n - 1) % 12 == 0:               # C natural: B below
        return n - 1
    return n - 1 if (n - 2) % 12 in DMAJ else n - 2


def plain(part, bar):
    seq(part, bar, E)


def ornamented(part, bar, bar_no, high):
    """The second time through: cuts on repeated notes, a roll on a long
    note, a slide up into the first high note of the high part."""
    prev = None
    for i, x in enumerate(bar):
        n, t = (x if isinstance(x, tuple) else (x, E))
        if t >= Q:
            # a roll: note, upper, note, lower, note
            m = note(n)
            seq(part, [(m, 4), (upper(m), 1), (m, 3), (lower(m), 1), (m, t - 9)], None)
        elif high and bar_no in (0, 4) and i == 0:
            # slide up a tone into it: a semitone every 2 ticks
            part.note(note(n) - 2, 0).glis(0x01).wait(4)
            part.glis_off().wait(t - 4)
        elif n == prev:
            part.note(upper(n), 1).note(n, t - 1)               # a cut
        else:
            part.note(n, t)
        prev = n


def build():
    s = Song("jig", "THE BRASS KETTLE", "CELTIC JIG", "D MAJOR", tempo=35, beat=DQ, beats=2, prompt=PROMPT)
    s.row = 6
    s.blurb = "AN IRISH JIG IN 6/8 WITH DRONES AND BODHRAN"
    F, Dr, B, D = s[0], s[1], s[2], s[3]
    bar = s.bar
    intro = 2

    # ---- Fiddle: two bars rest, then AABB ----
    F.rest(intro * bar)
    s.section(intro * bar, "A PART")
    F.vol(36).slide(-1).vib(3, 3)
    for b in A_PART:
        plain(F, b)
    s.section((intro + 8) * bar, "A AGAIN")
    for i, b in enumerate(A_PART):
        ornamented(F, b, i, False)
    s.section((intro + 16) * bar, "B PART")
    for b in B_PART:
        plain(F, b)
    s.section((intro + 24) * bar, "B AGAIN")
    for i, b in enumerate(B_PART):
        ornamented(F, b, i, True)

    # ---- Drone: D and A, alternating every tick, swelling ----
    Dr.vol(14).trem(1, 5).arp(0x70, 0x40)
    for _ in range(intro + 32):
        Dr.note("D4", bar)

    # ---- Bass: root, rest, fifth on every beat ----
    B.rest(intro * bar)
    B.vol(34).slide(-3)
    for chords in (A_CHORDS, A_CHORDS, B_CHORDS, B_CHORDS):
        for c in chords:
            for ch in c.split():
                r = note(ROOT[ch])
                seq(B, [r, "-", r + 7], E)

    # ---- Bodhran: DUM (a retriggered low thud), da, da; a roll to end each part ----
    kit = Kit(D, {"D": (1, 44, -6, 0x14), "l": (25, 12, -4), "m": (25, 20, -4), "r": (13, 16, -5)})
    beat = "Dlm"
    for i in range(intro + 32):
        last = (i - intro) % 8 == 7
        if i < intro:
            kit.pattern(beat * 2, E)
        elif last:
            kit.pattern("Dlm", E)
            for v in (10, 14, 18, 22, 28, 34):
                kit.hit("r", 3, v)
        else:
            kit.pattern(beat * 2, E)

    s.length = (intro + 32) * bar
    s.loop_at = intro * bar
    return s
