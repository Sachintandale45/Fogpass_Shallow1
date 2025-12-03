#!/usr/bin/env bash
set -euo pipefail

DEVICE=${1:-/dev/sda1}
MOUNT_POINT=${2:-/mnt/usb}

sudo mkdir -p "$MOUNT_POINT"
sudo mount "$DEVICE" "$MOUNT_POINT"
echo "Mounted $DEVICE on $MOUNT_POINT"
