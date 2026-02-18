#!/usr/bin/env bash
# Prepare LittleFS data partition for ESP32 flashing
# Run from firmware/ directory: ./scripts/prepare_data.sh

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
FIRMWARE_DIR="$(dirname "$SCRIPT_DIR")"
REPO_ROOT="$(dirname "$FIRMWARE_DIR")"
DATA_DIR="$FIRMWARE_DIR/data"

echo "=== TealClaw Data Partition Prep ==="
echo "Repo root: $REPO_ROOT"
echo "Data dir:  $DATA_DIR"

# Gzip index.html
if [ -f "$REPO_ROOT/index.html" ]; then
    echo "Compressing index.html..."
    gzip -9 -k -f "$REPO_ROOT/index.html"
    mv "$REPO_ROOT/index.html.gz" "$DATA_DIR/index.html.gz"
    echo "  -> data/index.html.gz ($(wc -c < "$DATA_DIR/index.html.gz") bytes)"
else
    echo "WARNING: index.html not found at $REPO_ROOT/index.html"
fi

# Copy static assets
ASSETS="manifest.json icon-192.png icon-512.png apple-touch-icon.png favicon-32.png"
for asset in $ASSETS; do
    if [ -f "$REPO_ROOT/$asset" ]; then
        cp "$REPO_ROOT/$asset" "$DATA_DIR/$asset"
        echo "  Copied $asset"
    else
        echo "  WARNING: $asset not found"
    fi
done

echo ""
echo "Data partition contents:"
ls -lh "$DATA_DIR/"
echo ""
echo "Done! Now run: pio run -t uploadfs"
