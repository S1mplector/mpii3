# Architecture

The application is written in C11. Each module has a narrow public header; implementation helpers and platform handles stay private. The portable core does not depend on GRRLIB, libogc, controller APIs, or rendering state.

| Module | Responsibility |
|---|---|
| `main` | Executable entry point only |
| `app` | Coordinates input, browsing, active queue, playback transitions, and settings panel |
| `discovery` | Recursively scans local storage, applies bounds, and sorts entries |
| `playlist` | Parses local M3U/M3U8 documents into a candidate queue |
| `queue` | Pure previous/next and loop policy |
| `path` | Internal bounded path operations and track-title extraction |
| `player` | Owns the audio stream, decoder lifecycle, pause/volume, and synchronized snapshots |
| `spectrum` | Portable energy normalization and time-based smoothing |
| `settings` | Visualizer defaults, validation, and adjustment rules |
| `config` | Settings file parsing and replace-on-success persistence |
| `input` | Maps Wii Remote events to semantic commands, including held-button scrolling |
| `view` | Builds the browser, now-playing, and settings screens from a read-only view model |
| `visualizer` | Groups measured bands and renders spectrum/orbit animations |
| `display` | Encapsulates GRRLIB, the font, logo texture, and drawing primitives |
| `text_encoding` | Bounded UTF-8 decoding with replacement for malformed filenames |

## Playback and ownership

The app owns the library and active queue. Opening a playlist first fills a separate candidate queue; a failed or empty playlist cannot discard the playing queue. Only `player` opens/closes the active MP3 and calls the audio library. It resumes paused output before stopping the worker, so buffered audio can drain during shutdown. Decoder callbacks update bands, elapsed decoded time, and read errors under a mutex; the UI reads snapshots.

Finished tracks advance according to loop policy. Unreadable tracks are skipped; errors never endlessly repeat a single broken track in loop-one mode. A full queue of failures stops after one pass. Elapsed time is based on decoded frames and may lead audible output by the audio buffer duration.

## Storage boundaries

Scanning and playback are read-only. Writes are confined to the selected app directory's `settings.ini` and temporary settings file. Saving first writes and closes the temporary file, then renames it over the prior file. Failure leaves the previous configuration intact. Filesystem-specific behavior requires the FAT hardware checks in VALIDATION.md.

No deployment is performed by build or packaging scripts. They create files under `build/` and `dist/`. No private music, console backups, keys, or system files belong in this repository.

## Graphics

GRRLIB handles GX rendering and TTF fonts. The display module converts filenames to wide characters itself, avoiding reliance on the Wii C locale for UTF-8. Logical layout uses 640×480 coordinates. Physical video modes and TV overscan remain hardware validation items.

The analyzer obtains energy from libmad's 32 decoded subbands. The visualizer groups those bands for 8/16/32 bars, applies sensitivity, then time-based attack/decay smoothing. Graphics never block on filesystem reads or inspect decoder internals.
