"""CHJukebox: what the shared tools need to know (the schema is in the
CHGame repository's tools/gamecfg.py; `chgame test`, `chgame check`, `chgame
redraw` read this). Everything is the default; the songs are checked by
tools/songs/make_songs.py (it plays each through the port and proves it
loops)."""
ECHO = ("STATE",)
PRE_STEPS = [("songs", ["tools/songs/make_songs.py"])]
SAVE_PAGES = 0                          # it saves nothing
