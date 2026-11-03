#!/bin/sh
set -eu

FLASH_BYTES=8388608
PORT=${1:-}
IMAGE=${2:-}

if [ -z "$PORT" ] || [ -z "$IMAGE" ]; then
  echo "Usage: $0 /dev/cu.usbserial-XXXX backups/geekmagic-smalltv-pro-stock-DATE.bin" >&2
  exit 2
fi
[ -f "$IMAGE" ] || { echo "Backup not found: $IMAGE" >&2; exit 1; }
SIZE=$(stat -f '%z' "$IMAGE")
[ "$SIZE" -eq "$FLASH_BYTES" ] || { echo "Refusing restore: image is $SIZE bytes, expected $FLASH_BYTES." >&2; exit 1; }
SHA=$(shasum -a 256 "$IMAGE" | awk '{print $1}')

echo "DANGER: THIS OVERWRITES THE COMPLETE 8 MB FLASH AT OFFSET 0x0."
echo "Port:    $PORT"
echo "Image:   $IMAGE"
echo "Bytes:   $SIZE"
echo "SHA-256: $SHA"
printf 'Type RESTORE-STOCK exactly to continue: '
read answer
[ "$answer" = "RESTORE-STOCK" ] || { echo "Cancelled. Nothing was written."; exit 1; }

python3 -m esptool --chip esp32 --port "$PORT" chip_id
python3 -m esptool --chip esp32 --port "$PORT" write_flash 0x0 "$IMAGE"
python3 -m esptool --chip esp32 --port "$PORT" verify_flash 0x0 "$IMAGE"

echo "Full stock image restored and verified."
echo "Disconnect power, remove GPIO0 from GND, then power the device normally."

