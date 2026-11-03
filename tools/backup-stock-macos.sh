#!/bin/sh
set -eu

FLASH_BYTES=8388608
STAMP=$(date '+%Y-%m-%d_%H-%M-%S')
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BACKUP_DIR="$ROOT/backups"
PORT=${1:-}
VERIFY=${2:-}

if [ -z "$PORT" ]; then
  echo "Usage: $0 /dev/cu.usbserial-XXXX [--verify-readback]" >&2
  echo "Available macOS serial ports:" >&2
  ls /dev/cu.* 2>/dev/null || true
  exit 2
fi

mkdir -p "$BACKUP_DIR"
DATED="$BACKUP_DIR/geekmagic-smalltv-pro-stock-$STAMP.bin"
STABLE="$BACKUP_DIR/stock-backup.bin"
META="$BACKUP_DIR/geekmagic-smalltv-pro-stock-$STAMP.txt"
SECOND="$BACKUP_DIR/.verify-$STAMP.bin"
trap 'rm -f "$SECOND"' EXIT HUP INT TERM

if [ -e "$DATED" ]; then
  printf 'Backup already exists: %s\nRefusing to overwrite it.\n' "$DATED" >&2
  exit 1
fi
if [ -e "$STABLE" ]; then
  printf 'The stable backup name already exists:\n  %s\nType OVERWRITE to replace only this convenience copy: ' "$STABLE"
  read answer
  [ "$answer" = "OVERWRITE" ] || { echo "Cancelled; existing backup was not changed."; exit 1; }
fi

echo "Connecting to $PORT. GPIO0 must be held to GND during power-on."
ESPTOOL_VERSION=$(python3 -m esptool version 2>&1 | head -1)
CHIP_INFO=$(python3 -m esptool --chip esp32 --port "$PORT" chip_id 2>&1)
printf '%s\n' "$CHIP_INFO"

python3 -m esptool --chip esp32 --port "$PORT" read_flash 0x0 0x800000 "$DATED"
[ -f "$DATED" ] || { echo "ERROR: esptool did not create the backup." >&2; exit 1; }
SIZE=$(stat -f '%z' "$DATED")
[ "$SIZE" -eq "$FLASH_BYTES" ] || { echo "ERROR: expected $FLASH_BYTES bytes, got $SIZE." >&2; exit 1; }
SHA=$(shasum -a 256 "$DATED" | awk '{print $1}')

if [ "$VERIFY" = "--verify-readback" ]; then
  echo "Performing an independent second 8 MB read..."
  python3 -m esptool --chip esp32 --port "$PORT" read_flash 0x0 0x800000 "$SECOND"
  SHA2=$(shasum -a 256 "$SECOND" | awk '{print $1}')
  [ "$SHA" = "$SHA2" ] || { echo "ERROR: readback hashes differ; do not flash anything." >&2; exit 1; }
  echo "Second read matches."
fi

cp -p "$DATED" "$STABLE"
{
  echo "device=GeekMagic SmallTV Pro"
  echo "detected_chip=$(printf '%s' "$CHIP_INFO" | tr '\n' ' ' | sed 's/[[:space:]][[:space:]]*/ /g')"
  echo "flash_size=8388608 bytes (8 MB)"
  echo "date=$(date '+%Y-%m-%dT%H:%M:%S%z')"
  echo "sha256=$SHA"
  echo "esptool_version=$ESPTOOL_VERSION"
  echo "file=$(basename "$DATED")"
  echo "second_read_verified=$([ "$VERIFY" = "--verify-readback" ] && echo yes || echo no)"
} > "$META"

echo
echo "VERIFIED STOCK BACKUP CREATED"
echo "Image:    $DATED"
echo "Stable:   $STABLE"
echo "Metadata: $META"
echo "Bytes:    $SIZE"
echo "SHA-256:  $SHA"
echo "Store the image and metadata safely before flashing custom firmware."

