#!/usr/bin/env python3
"""Build a Homebrew Channel ZIP. Never writes to removable storage."""
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
import hashlib
import shutil
import struct
import xml.etree.ElementTree as ET

root = Path(__file__).resolve().parents[1]
icon = root / "assets/icon.png"
assert icon.read_bytes()[:8] == b"\x89PNG\r\n\x1a\n"
assert struct.unpack(">II", icon.read_bytes()[16:24]) == (128, 48)
version = ET.parse(root / "assets/meta.xml").findtext("version")
out = root / "dist"
app = out / "apps/mpii3"
app.mkdir(parents=True, exist_ok=True)
for source, name in [(root / "build/mpii3.dol", "boot.dol"),
                     (icon, "icon.png"), (root / "assets/meta.xml", "meta.xml")]:
    shutil.copyfile(source, app / name)
archive = out / f"mpii3-{version}.zip"
with ZipFile(archive, "w", ZIP_DEFLATED) as package:
    for path in sorted(app.iterdir()):
        if path.name in {"boot.dol", "icon.png", "meta.xml"}:
            package.write(path, path.relative_to(out))
    for source in ["LICENSE", "THIRD_PARTY.md", "README.md", "assets/FONT-LICENSE.txt"]:
        package.write(root / source, f"apps/mpii3/licenses/{Path(source).name}")
checksum = hashlib.sha256(archive.read_bytes()).hexdigest()
(out / "SHA256SUMS").write_text(f"{checksum}  {archive.name}\n")
print(f"Packaged {archive.name}: {archive.stat().st_size:,} bytes")
