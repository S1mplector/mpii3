# Validation

## Automated

`make test` compiles the portable modules with AddressSanitizer and UndefinedBehaviorSanitizer. Fixtures exercise:

- Recursive discovery, mixed-case MP3 extensions, ignored files, and a symlink cycle.
- Relative paths, backslashes, UTF-8 BOM, CRLF, and EXTINF labels in playlists.
- Empty, missing, malformed, overlong, and capacity-limited playlists.
- Missing media, unsupported files, and network entries.
- Empty queues, boundaries, manual skip, loop-one, and loop-all transitions.
- Silence, nonfinite energy, and spectrum attack/decay.
- Settings bounds, malformed/nonfinite values, save/load roundtrips, existing-file replacement, and failed save paths.
- UTF-8 names, emoji, invalid sequences, truncation, and destination capacity.

`make` cross-compiles the app with warnings as errors. `make package` validates the Homebrew icon's PNG signature and 128×48 dimensions, packages the executable and metadata, and writes a SHA-256 checksum. GitHub Actions independently runs the host tests and Wii build.

## Real Wii checks still required

A successful compile is not proof of playback or rendering correctness. This alpha has not yet been run on a real Wii.

1. Launch from Homebrew Channel on SD, then USB. Check 4:3 / 16:9 and PAL / NTSC layouts.
2. Play mono/stereo, CBR/VBR, and 32 / 44.1 / 48 kHz MP3s. Listen for underruns during browsing and settings changes.
3. Pause/resume, skip rapidly, reach EOF, exit while paused, and repeat these transitions.
4. Load a relative-path playlist. Test off/all/one looping, unreadable files, an entirely invalid queue, and removal of storage while playing.
5. Observe the spectrum with silence, bass-heavy music, and high-frequency test audio. Verify settings change the display immediately.
6. Save settings, relaunch, and confirm persistence on FAT. Test an unwritable app directory and interrupted saves.
7. Leave a long playlist running and check memory stability, controller reconnect, and exit behavior.

No custom app has been copied onto the user's Wii card during development. The earlier WiiMC/music installation is separate from this repository.
