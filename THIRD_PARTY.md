# Third-party components

- **GRRLIB**: pinned under `external/grrlib`; MIT license in the submodule. Includes PNGU support. Source: https://github.com/GRRLIB/GRRLIB
- **devkitPro / libogc**: Wii toolchain and platform libraries. Source and component licenses: https://github.com/devkitPro/libogc and https://github.com/devkitPro/buildscripts
- **libmad**: GPL-2.0-or-later MP3 decoder, linked from the devkitPro SDK. The project uses GPL-2.0-or-later to accommodate this dependency. The libogc MP3 wrapper is provided by the SDK.
- **FreeType, libpng, libjpeg, zlib, bzip2, Brotli**: SDK font/image dependencies, retaining their upstream licenses. Build dependencies are distributed in the official devkitPro image pinned in `.github/workflows/build.yml`.
- **DejaVu Sans**: `assets/ui.ttf`; the unmodified font is bundled under its permissive font license. Full notices are in `assets/FONT-LICENSE.txt`.

The application source for each build is available in this repository at the corresponding commit. GRRLIB's exact source revision is recorded by the Git submodule. The pinned SDK container identifies the build environment. No third-party songs or console system files are included.
