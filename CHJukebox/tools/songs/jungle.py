"""CIRCUIT BREAKER: mid-90s jungle / liquid drum & bass."""
from atmgen import Song, note
from music import Kit

PROMPT = """
The brief: "Mid-90s jungle at 172 BPM in F minor, the liquid kind. The
noise channel is the star: a chopped breakbeat in the spirit of the Amen,
with ghost snares between the hits, a kick that thuds, a 32nd-note snare
roll and a break edit every fourth bar. Under it a 'reese' bass on the saw:
long, sustained notes with a slow vibrato for that detuned wobble. The
square plays a dreamy pad of slow three-note arpeggios with tremolo over
Fm7 - Dbmaj7 - Bbm7 - C7, and the pulse a sparse lead whose every note is
echoed twice, quieter, a dotted eighth apart: a dub delay made of notes.
Form: four bars of intro (pad, hats and ghost notes, the lead far away, a
noise riser into the drop), the drop (eight bars), a half-time switch-up
where the bass dives, and the second drop with the lead an octave up."
CH0 pulse: the lead and its echoes. CH1 square: the pad. CH2 saw: the
reese. CH3 noise: the break.
"""

S16 = 4
CHORDS = [("F3", 0x43, "Ab4"), ("Db3", 0x34, "F4"), ("Bb2", 0x43, "Db5"), ("C3", 0x33, "E4")]   # a bar each


def echoes(p, events, length, vols=(30, 13, 5), gap=12):
    """Lead notes, each repeated quieter `gap` ticks later (until the next note)."""
    hits = []
    for i, (t, n) in enumerate(events):
        nxt = events[i + 1][0] if i + 1 < len(events) else length
        for k, v in enumerate(vols):
            if t + k * gap < nxt:
                hits.append((t + k * gap, n, v))
    start = p.t
    for i, (t, n, v) in enumerate(hits):
        p.to(start + t)
        p.vol(v)
        p.note(n, 0)
    p.to(start + length)


def build():
    s = Song("jungle", "CIRCUIT BREAKER", "DRUM & BASS", "F MINOR", tempo=46, beat=16, beats=4, prompt=PROMPT)
    s.blurb = "CHOPPED BREAKBEATS, REESE BASS, DUB ECHOES"
    L, P, B, D = s[0], s[1], s[2], s[3]
    bar = s.bar
    intro = 4

    # ---- Pad: slow arpeggios with tremolo, a chord a bar ----
    P.vol(16).trem(1, 7)
    for i in range(intro + 20):
        _, arp, base = CHORDS[i % 4]
        P.arp(arp, 0x23).note(base, bar)

    # ---- Reese ----
    B.rest(intro * bar)
    s.section(intro * bar, "DROP")
    B.vol(52).slide(0).vib(1, 7)
    for i in range(8):                                  # the drop: held roots, a push on the "a" of 2
        r = note(CHORDS[i % 4][0])
        B.note(r, 28).note(r, 12).note(r + 12 if i % 2 else r + 7, 8).note(r, 16)
    s.section((intro + 8) * bar, "SWITCH UP")
    for i in range(4):                                  # the switch-up: dives
        r = note(CHORDS[i][0])
        B.note(r + 12, 32).fslide(-10).wait(24).fslide_off().rest(8)
    s.section((intro + 12) * bar, "DROP II")
    for i in range(8):
        r = note(CHORDS[i % 4][0])
        if i % 4 == 3:
            B.note(r, 16).note(r + 3, 16).note(r + 7, 16).note(r + 10, 16)     # a walk up into the top
        else:
            B.note(r, 28).note(r, 12).note(r + 12 if i % 2 else r + 7, 8).note(r, 16)

    # ---- Lead: a phrase every two bars, echoed ----
    phrase = [(0, "C6"), (36, "Eb6"), (64, "F6"), (88, "Eb6"), (100, "C6")]
    answer = [(0, "Ab5"), (36, "Bb5"), (64, "C6"), (100, "G5")]
    L.slide(-1)
    for i in range(intro // 2):
        echoes(L, phrase if i == 0 else answer, 2 * bar, vols=(14, 6, 2))
    for i in range(4):
        echoes(L, phrase if i % 2 == 0 else answer, 2 * bar)
    L.rest(2 * bar)
    echoes(L, [(32, "F5"), (48, "Ab5"), (64, "C6"), (96, "Bb5")], 2 * bar)
    high = [(0, "F6"), (36, "Ab6"), (64, "Bb6"), (88, "Ab6"), (100, "F6")]
    high_answer = [(0, "C6"), (36, "Eb6"), (64, "F6"), (88, "G6"), (100, "C6")]
    for i in range(4):
        echoes(L, (high, high_answer, phrase, answer)[i], 2 * bar)

    # ---- The break ----
    kit = Kit(D, {"K": (1, 54, -12, 0x14), "S": (3, 48, -4), "g": (4, 16, -4), "h": (42, 10, -5),
                  "x": (3, 30, -6), "w": (49, 2, 1)})
    A1 = "K.h. S.hg .hK. S.hg"
    A2 = "K.h. S.hg .hKK .gS."
    for i in range(intro):
        if i < intro - 1:
            kit.pattern("h.h. g.h. h.hg g.h." if i % 2 else "h.h. g.h. h.h. g.hg", S16)
        else:
            kit.hit("w", 62)                            # the riser: noise swelling into the drop
            kit.hit("x", 2)
    for i in range(16):
        if i >= 8 and i < 12:                           # the switch-up: half time
            kit.pattern("K.h. h.h. S.h. h.hg" if i % 2 == 0 else "K.h. h.K. S.h. hghS", S16)
        elif i % 4 == 3:                                # the edit: a roll
            kit.pattern("K.h. S.hg", S16)
            for v in (18, 22, 26, 30, 34, 38, 42, 46):
                kit.hit("x", 2, v)
            kit.pattern("S.SK", S16)
        else:
            kit.pattern(A1 if i % 2 == 0 else A2, S16)
    for i in range(4):
        if i < 3:
            kit.pattern(A1 if i % 2 == 0 else A2, S16)
        else:                                           # the last edit: a longer roll back to the top
            kit.pattern("K.h. S.hg K.S.", S16)
            for v in (14, 20, 26, 32, 38, 44, 50, 56):
                kit.hit("x", 2, v)

    s.length = (intro + 20) * bar
    s.loop_at = intro * bar
    return s
