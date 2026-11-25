"""Drive CHJukebox - in the simulator or on the device - with a script.

    chgame run <script> <outdir>
    python tools/chsim/chdrive.py --device [--port COMx] <script> <outdir>

The repository's tools/chsim/chdrivelib.py does the driving and has the
common script commands (wait, tap, hold, snap, gif, rec, say, perf, cal ...).
CHJukebox adds:
    state               print what is on screen (the H command): the screen,
                        the song, the view, the tick, the pass, the section,
                        the engine, the drive, paused or not
In the simulator, $ATM_WAV names a WAV file to get the synth's output (the
sound of the run, 15,625 Hz 8-bit).
--id names the sketch's handshake reply (default JUKE).
"""
import os
import sys
from pathlib import Path


def _tools():
    """The CHGame tools: $CHGAME_ROOT/tools, else the tools/ of a CHGame
    checkout this sketch sits in, or of one beside this project (../CHGame)."""
    root = os.environ.get("CHGAME_ROOT")
    cands = [Path(root) / "tools"] if root else []
    for up in Path(__file__).resolve().parents:
        cands += [up / "tools", up / "CHGame" / "tools"]
    for c in cands:
        if (c / "chsim" / "chdrivelib.py").exists():
            return c
    raise SystemExit(f"{Path(__file__).name}: the CHGame tools (tools/chsim/chdrivelib.py) were not found; "
                     "set CHGAME_ROOT to a CHGame checkout (BUILDING.md)")


sys.path.insert(0, str(_tools() / "chsim"))
from chdrivelib import Driver, SerialTransport, SimTransport, main, mask_of  # noqa: E402,F401


class JukeDriver(Driver):
    def op(self, name, args, outdir):
        if name == "state":
            print(self.query("H", "STATE"), flush=True)
            return True
        return False


DRIVER, IDENT = JukeDriver, "JUKE"        # what the shared tools load from this file

if __name__ == "__main__":
    main(DRIVER, ident=IDENT)
