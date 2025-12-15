# This file is placed in recipes-kernel/linux/ and targets the main RPi kernel recipe.

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Add the mcp3008-overlay.dts file to the kernel's source files.
# We use :append to safely add to the existing SRC_URI list.
SRC_URI:append = " \
    file://mcp3008-overlay.dts \
    file://0001-enable-spidev.cfg \
"

# Specify the new overlay file to be compiled into a Device Tree Blob (.dtbo)
# and included in the final image's boot directory.
DTS_FILE_OVERLAYS:append = " mcp3008-overlay.dts"
