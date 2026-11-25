"""RAIN ON GLASS: a late-night lo-fi jazz ballad."""
from atmgen import Song, note
from music import Kit

PROMPT = """
The brief: "A late-night lo-fi jazz ballad at 75 BPM with a lazy triplet
swing, in Db major. Lush extended harmony played as slow rolled
arpeggios, each chord voiced by its colour tones above the bass:
Dbmaj9 (Ab-C-Eb), Bbm9, Ebm9, Ab13 (Gb-C-F), Gbmaj7, then the bittersweet
backdoor Gbm6, Fm7 to Bb7b9, and Ebm9 to D7, the tritone substitute
that slides home to Db. A walking bass, a quarter a beat, chord tones on
the strong beats and chromatic approach notes into every change. The
pulse sings a muted-trumpet melody, laid back behind the beat, held notes
with a slow vibrato, scoops up into the high ones, a blue note in the
second chorus and a climb to a high Bb at its peak. Brushes: a swish on 2
and 4, a swung ride pattern, and the faint crackle of a record. Form: two
bars of intro, the 8-bar tune, and the tune again, embellished and higher."
CH0 pulse: the melody. CH1 square: the chords. CH2 saw: the walking bass.
CH3 noise: brushes and crackle.
"""

BEAT = 24
BAR = 96
# (bass root for the voicing, arpeggio, base note, ticks)
INTRO_CHORDS = [(0x43, "Ab4", 96), (0x65, "Gb4", 96)]
CHORDS = [
    [(0x43, "Ab4", 96)],                     # Dbmaj9
    [(0x34, "F4", 96)],                      # Bbm9
    [(0x34, "Bb4", 96)],                     # Ebm9
    [(0x65, "Gb4", 96)],                     # Ab13
    [(0x34, "Bb4", 96)],                     # Gbmaj7
    [(0x42, "A4", 96)],                      # Gbm6
    [(0x43, "Ab4", 48), (0x33, "Ab4", 48)],  # Fm7 | Bb7b9
    [(0x34, "Bb4", 48), (0x65, "C5", 48)],   # Ebm9 | D7(13)
]
INTRO_BASS = [("Db3", 48), ("Ab2", 48), ("Ab2", 48), ("Eb3", 24), ("D3", 24)]
WALK = [
    ["Db3", "F3", "Ab3", "A3"],
    ["Bb3", "Ab3", "F3", "E3"],
    ["Eb3", "F3", "Gb3", "G3"],
    ["Ab2", "C3", "Eb3", "F3"],
    ["Gb3", "Bb3", "Db4", "Bb3"],
    ["Gb3", "A3", "Db3", "E3"],
    ["F3", "C3", "Bb2", "D3"],
    ["Eb3", "Bb2", "D3", "Ab2"],
]
# The tune: (note, ticks); "-" rests, "/X" scoops up into X.
TUNE = [
    [("-", 16), ("Ab5", 8), ("C6", 48), ("Bb5", 16), ("Ab5", 8)],
    [("F5", 48), ("Eb5", 16), ("F5", 8), ("Ab5", 24)],
    [("Gb5", 16), ("F5", 8), ("Eb5", 16), ("Db5", 8), ("Bb5", 48)],
    [("/C6", 24), ("Bb5", 16), ("Ab5", 8), ("F5", 48)],
    [("-", 16), ("Bb5", 8), ("Db6", 16), ("Eb6", 8), ("F6", 48)],
    [("Eb6", 16), ("Db6", 8), ("A5", 48), ("Ab5", 24)],
    [("Ab5", 16), ("F5", 8), ("C6", 24), ("B5", 16), ("Ab5", 8), ("F5", 24)],
    [("Gb5", 16), ("F5", 8), ("Eb5", 24), ("C6", 16), ("A5", 8), ("F#5", 24)],
]
TUNE2 = [
    [("-", 8), ("Ab5", 8), ("Bb5", 8), ("C6", 24), ("Eb6", 16), ("Db6", 8), ("C6", 16), ("Ab5", 8)],
    [("F5", 24), ("E5", 16), ("F5", 8), ("Ab5", 16), ("C6", 8), ("Db6", 24)],
    [("Eb6", 16), ("Db6", 8), ("Bb5", 16), ("Gb5", 8), ("F5", 16), ("Eb5", 8), ("Db5", 24)],
    [("C5", 16), ("Eb5", 8), ("F5", 16), ("Ab5", 8), ("/C6", 24), ("Bb5", 24)],
    [("Db6", 16), ("F6", 8), ("/Bb6", 48), ("Ab6", 16), ("F6", 8)],
    [("Ab6", 16), ("Gb6", 8), ("Eb6", 24), ("Db6", 16), ("A5", 8), ("Bb5", 24)],
    [("Ab5", 16), ("G5", 8), ("F5", 24), ("B5", 16), ("D6", 8), ("F6", 24)],
    [("Eb6", 16), ("Db6", 8), ("Bb5", 24), ("C6", 16), ("A5", 8), ("F#5", 24)],
]


def melody(p, bars):
    for line in bars:
        for n, t in line:
            if n == "-":
                p.rest(t)
            elif n[0] == "/":
                m = note(n[1:])
                p.note(m - 2, 0).glis(0x02).wait(6).glis_off().wait(t - 6)
            else:
                p.note(n, t)


def brushes(kit, bar_no):
    """Swung ride taps, a swish on 2 and 4, and a few crackles (four
    patterns, so the bars repeat and share tracks)."""
    crackle = [(10, 58, 64), (30, 61, 83), (5, 54, 77), (19, 66, 91)][bar_no % 4]
    hits = [(0, "r"), (24, "s"), (40, "a"), (48, "r"), (72, "s"), (88, "a")]
    hits += [(t, "c") for t in crackle]
    hits.sort()
    p = kit.p
    start = p.t
    for i, (t, inst) in enumerate(hits):
        end = hits[i + 1][0] if i + 1 < len(hits) else BAR
        p.to(start + t)
        kit.hit(inst, end - t)


def build():
    s = Song("nocturne", "RAIN ON GLASS", "LO-FI JAZZ", "DB MAJOR", tempo=30, beat=BEAT, beats=4, prompt=PROMPT)
    s.row = 8
    s.blurb = "A LATE-NIGHT BALLAD, EXTENDED CHORDS, BRUSHES"
    M, P, B, D = s[0], s[1], s[2], s[3]
    intro = 2

    # ---- Chords ----
    P.vol(15).trem(1, 11)
    for arp, base, t in INTRO_CHORDS:
        P.arp(arp, 0x22).note(base, t)
    for _ in range(2):
        for bar in CHORDS:
            for arp, base, t in bar:
                P.arp(arp, 0x22).note(base, t)

    # ---- Bass ----
    B.vol(40).slide(-1)
    for n, t in INTRO_BASS:
        B.note(n, t)
    for _ in range(2):
        for bar in WALK:
            for n in bar:
                B.note(n, BEAT)

    # ---- Melody ----
    M.rest(intro * BAR)
    s.section(intro * BAR, "THE TUNE")
    M.vol(28).slide(0).vib(2, 5)
    melody(M, TUNE)
    s.section((intro + 8) * BAR, "AGAIN, HIGHER")
    melody(M, TUNE2)

    # ---- Brushes ----
    kit = Kit(D, {"r": (51, 10, -1), "a": (51, 6, -1), "s": (38, 14, -1), "c": (25, 5, -5)})
    for i in range(intro + 16):
        brushes(kit, i)

    s.length = (intro + 16) * BAR
    s.loop_at = intro * BAR
    return s
