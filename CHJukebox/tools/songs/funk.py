"""VELVET STRUT: late-70s disco-funk."""
from atmgen import Song, note
from music import seq, Kit

PROMPT = """
The brief: "Late-70s disco-funk at 109 BPM in E dorian. The bass is the
star: a slap-style line on the saw that thumbs the root on the one, pops the
octave, and walks up chromatically (G, G#, A) into the third beat. A
'chicken-scratch' guitar on the square: muted sixteenth chops, each chord a
fast three-note arpeggio in a real funk voicing (Em9 as B-D-F#, A13 as the
quartal C#-F#-B, Cmaj7, Bm7, Am7, and a Hendrix B7#9 for the turnaround).
Four-on-the-floor disco drums on the noise channel: a thudding kick on
every beat, a crack on 2 and 4, open hats on the off-beats, fills every
fourth bar. The pulse plays a sassy, bluesy hook with slides up into notes
and vibrato on the held ones, answered by the bass. Form: two bars of
drums and guitar to count it in, then the groove (Em9 - A13, the lead only
answering), the hook over Cmaj7 - Bm7 - Am7 - B7#9, a breakdown where only
bass and hats are left, and the hook again to bring it home."
CH0 pulse: the lead. CH1 square: the guitar. CH2 saw: the bass.
CH3 noise: the drums.
"""

S16 = 4                 # a sixteenth note


def bass_bar(p, root, q="m", walk=True):
    """One bar of the groove on root: thumb, pop, the chromatic walk-up.
    q: 'm' minor 7, '7' dominant (the walk is #9 to 3), 'M' major 7."""
    r = note(root)
    if walk:
        w = (r + 2, r + 4) if q == "M" else (r + 3, r + 4)
        sev = r + 11 if q == "M" else r + 10
        seq(p, [(r, 8), (r + 12, 4), "-",
                "-", r, w[0], w[1],
                r + 5, "-", r + 12, sev,
                "-", r + 7, r + 9 if q == "M" else sev, r + 12], S16)
    else:                                       # the breakdown: sparser, deeper
        seq(p, [(r, 8), (r + 12, 4), "-",
                ("-", 8), r + 10, r + 12,
                ("-", 8), (r, 4), "-",
                r + 7, "-", r + 10, r + 12], S16)


def chops(p, arp, base, pattern="x.xx.x.xx.xx.x.x"):
    """Muted sixteenths: 3 ticks of the chord's arpeggio, 1 of silence."""
    p.arp(arp, 0x20)
    for c in pattern:
        if c == "x":
            p.note(base, 3).rest(1)
        else:
            p.rest(S16)


# (bass root, quality, guitar arpeggio, its base note)
GROOVE = [("E3", "m", 0x34, "B4"), ("E3", "m", 0x34, "B4"), ("A3", "7", 0x55, "C#5"), ("A3", "7", 0x55, "C#5")] * 2
HOOK = [("C3", "M", 0x34, "E4"), ("B2", "m", 0x34, "D4"), ("A2", "m", 0x43, "C4"), ("B2", "7", 0x65, "D#4")] * 2

# The lead's answers in the groove (bar index -> line), silent elsewhere.
LICKS = {
    1: [("-", 32), ("E5", 4), ("G5", 4), ("-", 4), ("A5", 4), ("B5", 8), ("A5", 4), ("G5", 4)],
    3: [("-", 32), ("C#6", 4), ("-", 4), ("B5", 4), ("A5", 4), ("E5", 16)],
    5: [("-", 32), ("B5", 4), ("D6", 4), ("-", 4), ("E6", 12), ("D6", 8)],
    7: [("-", 16), ("E5", 4), ("F#5", 4), ("G5", 4), ("G#5", 4), ("A5", 4), ("B5", 4), ("D6", 4), ("E6", 4),
        ("F#6", 8), ("-", 8)],
}
# The hook: four bars over Cmaj7 - Bm7 - Am7 - B7#9. "/" slides up a tone into the note.
HOOK_LEAD = [
    [("G5", 4), ("A5", 4), ("B5", 8), ("-", 4), ("D6", 8), ("B5", 4), ("/E6", 12), ("D6", 4), ("B5", 4), ("A5", 4), ("G5", 8)],
    [("F#5", 12), ("A5", 4), ("B5", 16), ("-", 8), ("A5", 4), ("B5", 4), ("D6", 8), ("B5", 8)],
    [("C6", 8), ("B5", 4), ("A5", 4), ("G5", 8), ("E5", 8), ("G5", 4), ("A5", 12), ("-", 16)],
    [("D6", 4), ("D#6", 12), ("B5", 8), ("A5", 8), ("F#5", 8), ("A5", 4), ("B5", 4), ("/D6", 16)],
]
HOOK_ANSWER = [   # the second time: the last two bars answer instead
    [("C6", 8), ("B5", 4), ("A5", 4), ("G5", 4), ("A5", 4), ("E5", 8), ("-", 8), ("E6", 4), ("D6", 4), ("B5", 16)],
    [("A5", 4), ("B5", 4), ("D#6", 8), ("F#6", 8), ("D#6", 4), ("B5", 4), ("/E6", 32)],
]


def lead(p, line):
    for n, t in line:
        if n == "-":
            p.rest(t)
        elif n[0] == "/":
            m = note(n[1:])
            p.note(m - 2, 0).glis(0x01).wait(4).glis_off().wait(t - 4)
        else:
            p.note(n, t)


def build():
    s = Song("funk", "VELVET STRUT", "DISCO FUNK", "E DORIAN", tempo=29, beat=16, beats=4, prompt=PROMPT)
    s.blurb = "SLAP BASS, CHICKEN-SCRATCH GUITAR, FOUR ON THE FLOOR"
    L, G, B, D = s[0], s[1], s[2], s[3]
    bar = s.bar
    intro = 2

    # ---- Guitar ----
    G.vol(20).slide(0)
    for _ in range(intro):
        chops(G, 0x34, "B4")
    for _, _, arp, base in GROOVE:
        chops(G, arp, base)
    for _, _, arp, base in HOOK:
        chops(G, arp, base, "x.xx.x.xx.x.xxx.")
    G.rest(4 * bar)                                  # the breakdown
    for _, _, arp, base in HOOK[:3]:
        chops(G, arp, base, "x.xx.x.xx.x.xxx.")
    chops(G, 0x65, "D#4", "x.x.x.x.x...x...")   # the stabs into the top

    # ---- Bass ----
    B.rest(intro * bar)
    B.vol(46).slide(-4)
    for root, q, _, _ in GROOVE + HOOK:
        bass_bar(B, root, q)
    for _ in range(4):
        bass_bar(B, "E3", walk=False)
    for root, q, _, _ in HOOK[:4]:
        bass_bar(B, root, q)

    # ---- Lead ----
    L.rest(intro * bar)
    s.section(intro * bar, "GROOVE")
    L.vol(30).slide(0).vib(4, 3)
    for i in range(8):
        lead(L, LICKS.get(i, [("-", bar)]))
    s.section((intro + 8) * bar, "HOOK")
    for line in HOOK_LEAD + HOOK_LEAD[:2] + HOOK_ANSWER:
        lead(L, line)
    s.section((intro + 16) * bar, "BREAKDOWN")
    lead(L, [("-", 32), ("B5", 32)])
    lead(L, [("A5", 8), ("G5", 8), ("E5", 48)])
    lead(L, [("-", 32), ("/E6", 16), ("D6", 8), ("B5", 8)])
    lead(L, [("D6", 8), ("B5", 8), ("A5", 8), ("G5", 8), ("A5", 4), ("B5", 4), ("-", 24)])
    s.section((intro + 20) * bar, "HOOK")
    for line in HOOK_LEAD:
        lead(L, line)

    # ---- Drums ----
    kit = Kit(D, {"K": (1, 52, -10, 0x14), "S": (3, 42, -4), "h": (42, 9, -4), "O": (46, 18, -2)})
    beat_bar = "KhOh ShOh KhOh ShOh"
    fill_bar = "KhOh ShOh KhSS ShSS"
    for i in range(intro):
        kit.pattern(beat_bar if i == 0 else fill_bar, S16)
    for i in range(16):
        kit.pattern(fill_bar if i % 4 == 3 else beat_bar, S16)
    for i in range(4):
        kit.pattern("hhOh hhOh hhOh hhOh" if i < 3 else "hhOh hhOh SSSS SSSS", S16)
    for i in range(4):
        kit.pattern(fill_bar if i == 3 else beat_bar, S16)

    s.length = (intro + 24) * bar
    s.loop_at = intro * bar
    return s
