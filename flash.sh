#!/bin/bash


set -e


if [ $# -ne 2 ]; then
echo "Usage: sudo $0 <build-dir> /dev/sdX"
exit 1
fi


BUILD_DIR="$1"
DEVICE="$2"


IMAGE_DIR="$BUILD_DIR/tmp/deploy/images/raspberrypi4"
IMAGE_BZ2="$IMAGE_DIR/core-image-minimal-raspberrypi4.rootfs.wic.bz2"
OUTPUT_IMAGE="$IMAGE_DIR/core-image-minimal.wic"


if [[ ! -d "$IMAGE_DIR" ]]; then
echo "Error: Image directory not found: $IMAGE_DIR"
exit 1
fi


if [[ ! -f "$IMAGE_BZ2" ]]; then
echo "Error: Image file not found: $IMAGE_BZ2"
exit 1
fi


if [[ ! -b "$DEVICE" ]]; then
echo "Error: $DEVICE is not a valid block device"
exit 1
fi


if [ "$EUID" -ne 0 ]; then
echo "Error: This script must be run as root."
echo "Use: sudo $0 $BUILD_DIR $DEVICE"
exit 1
fi


echo "WARNING: ALL DATA on $DEVICE will be ERASED!"
read -p "Type YES to continue: " CONFIRM


if [ "$CONFIRM" != "YES" ]; then
echo "Aborted."
exit 1
fi


echo "Unmounting existing partitions on $DEVICE..."
for p in $(ls ${DEVICE}?* 2>/dev/null); do
umount $p 2>/dev/null || true
done


echo "Decompressing image..."
bunzip2 -c "$IMAGE_BZ2" > "$OUTPUT_IMAGE"


if [ ! -f "$OUTPUT_IMAGE" ]; then
echo "Decompression failed"
exit 1
fi


echo "Flashing image to $DEVICE..."
dd if="$OUTPUT_IMAGE" of="$DEVICE" bs=4M status=progress conv=fsync
sync


echo "Flash completed successfully ✅"
echo "You can now safely remove the SD card"
