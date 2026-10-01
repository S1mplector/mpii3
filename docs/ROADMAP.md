# Development roadmap

## Implemented for the first alpha

- Local discovery and deterministic sorting.
- MP3 playback and queue transitions.
- M3U/M3U8 parsing and off/all/one repeat modes.
- GRRLIB UI, Aero SVG logo, and Wii PNG icon.
- Spectrum/orbit visuals, 8/16/32 bars, sensitivity, response, and smoothing.
- Settings persistence, core tests, Wii build, CI, and Homebrew packaging.

## Next gate: hardware validation

Complete the checks in VALIDATION.md before claiming stable Wii playback. Prioritize decoder lifecycle, pause/skip/exit, SD/USB I/O failure, FAT settings replacement, and frame rate.

## Following increments

- Shuffle and a dedicated now-playing / fullscreen visualizer screen.
- Album and artist tags, track duration, and album artwork.
- Cached font glyphs if profiling shows text rendering is too costly.
- Rescan storage without restarting; responsive background discovery.
- Seek controls and additional visualizer styles.
- Optional Wii Menu forwarder only after the app is stable.
