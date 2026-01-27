SUMMARY = "C application to read from MCP3008 ADC via /dev/spidev"
DESCRIPTION = "Reads the raw 10-bit value from an analog sensor channel."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Assumes main.c is placed in the 'files' subdirectory
SRC_URI = "file://main.c"

S = "${WORKDIR}"


# Simple compilation using GCC
do_compile() {
    # Ensure the code links against the standard C library
    ${CC} ${CFLAGS}  ${LDFLAGS} -o adc ${WORKDIR}/main.c -Wall
}

# Install the executable into the /usr/bin directory on the target
do_install() {
    install -d ${D}${bindir}
    install -m 0755 adc ${D}${bindir}
}
