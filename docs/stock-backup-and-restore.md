# SmallTV Pro stock backup and restore on macOS

> **DO NOT FLASH CUSTOM FIRMWARE UNTIL A VERIFIED STOCK BACKUP EXISTS.**

The SmallTV Pro USB-C socket supplies power only. A complete backup requires opening the case, a **3.3 V** USB-UART adapter and the internal UART header. Never use 5 V logic.

| Header pin | Connect to |
|---|---|
| 1 GND | adapter GND |
| 2 TX | adapter RX |
| 3 RX | adapter TX |
| 4 3V3 | adapter 3.3 V, if the adapter safely powers the board |
| 5 GPIO0 | GND during power-on to enter download mode |
| 6 RST | briefly to GND to reset, if needed |

Install esptool if it is not already present: `python3 -m pip install --user esptool`.

List likely ports with `ls /dev/cu.*`. On macOS use `/dev/cu.*` rather than `/dev/tty.*` for an outgoing serial connection. CH340 adapters often appear as `/dev/cu.wchusbserial*`; CP210x adapters as `/dev/cu.SLAB_USBtoUART`; FTDI adapters as `/dev/cu.usbserial-*`.

## Create and verify the complete 8 MB backup

Hold GPIO0 at GND while powering or resetting the unit, then run:

```bash
./tools/backup-stock-macos.sh /dev/cu.usbserial-XXXX --verify-readback
```

The underlying first read is exactly:

```bash
python3 -m esptool --chip esp32 --port /dev/cu.usbserial-XXXX read_flash 0x0 0x800000 backups/stock-backup.bin
```

The helper refuses accidental overwrites, requires exactly 8,388,608 bytes, prints SHA-256, optionally performs a second independent read, and writes a dated image plus metadata. Copy both to safe storage. Do not proceed if the two read hashes differ.

## Restore the complete stock image

This is destructive. Re-enter download mode and run only with your own verified image:

```bash
./tools/restore-stock-macos.sh /dev/cu.usbserial-XXXX backups/geekmagic-smalltv-pro-stock-YYYY-MM-DD_HH-MM-SS.bin
```

The underlying write is:

```bash
python3 -m esptool --chip esp32 --port /dev/cu.usbserial-XXXX write_flash 0x0 backups/geekmagic-smalltv-pro-stock-YYYY-MM-DD_HH-MM-SS.bin
```

The script validates size and SHA-256, requires typing `RESTORE-STOCK`, writes at offset `0x0`, and runs `verify_flash`. It never modifies the source backup.

If esptool cannot connect, confirm TX/RX are crossed, grounds are common, the adapter uses 3.3 V logic, GPIO0 was low at reset, and no other program has the serial port open. A repeated `Connecting...` usually means download mode was not entered; `Permission denied` or a missing port usually means a cable/driver/port-name issue.

## First custom installation after backup

After the backup is verified, disconnect UART, remove GPIO0 from GND, and boot stock firmware normally. Browse to `http://DEVICE-IP/update` and upload `smalltv-mod-firmware-esp32-pro.bin`. This OTA application image does not erase the complete flash. Do not upload the `.factory.bin` file through the web updater.

