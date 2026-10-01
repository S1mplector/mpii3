#!/usr/bin/env python3
"""Optional: pip install cairosvg, then regenerate the checked-in PNG assets."""
from pathlib import Path
import cairosvg
assets = Path(__file__).resolve().parents[1] / "assets"
cairosvg.svg2png(url=str(assets / "icon.svg"), write_to=str(assets / "icon.png"))
cairosvg.svg2png(url=str(assets / "logo.svg"), write_to=str(assets / "logo.png"),
                output_width=256, output_height=256)
