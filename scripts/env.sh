# Source this file: . scripts/env.sh
if [ -z "${DEVKITPRO:-}" ]; then
    if [ -d "$HOME/.local/share/mpii3-sdk/opt/devkitpro" ]; then
        export DEVKITPRO="$HOME/.local/share/mpii3-sdk/opt/devkitpro"
    else
        export DEVKITPRO=/opt/devkitpro
    fi
fi
export DEVKITPPC="$DEVKITPRO/devkitPPC"
export PATH="$DEVKITPRO/tools/bin:$DEVKITPPC/bin:$PATH"
