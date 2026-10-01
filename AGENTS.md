# mpii3 development

Keep modules focused and encapsulated. Use GRRLIB for UI rendering. Keep filesystem parsing, queue policies, settings, and spectrum math portable and testable. Do not introduce a dependency on rendering into playback or discovery.

Run `make test` for changes to the portable core and a Wii cross-build for application changes. Source `scripts/env.sh` to select an installed SDK. Package with `make package`.

The SVG logo uses a late-2000s Aero glass style. Keep `assets/icon.svg` and its 128x48 PNG in sync. The full logo is `assets/logo.svg` with a 256x256 PNG export.

Never modify removable storage as part of development or testing. Deployment requires a user request. Keep music, console backups, keys, and system files out of this public repository. Do not claim hardware playback has been verified until it has been tested on a real Wii.
