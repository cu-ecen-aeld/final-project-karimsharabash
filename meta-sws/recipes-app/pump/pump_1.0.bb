SUMMARY = "C application to control the GPIO which controls the pump"
DESCRIPTION = "Userspace CLI tool to control a water pump via GPIO using libgpiod"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Assumes main.c is placed in the 'files' subdirectory
SRC_URI = "file://pump.c"

S = "${WORKDIR}"

DEPENDS += "libgpiod"
RDEPENDS:${PN} += "libgpiod"

# Simple compilation using GCC
do_compile() {
    # Ensure the code links against the standard C library
    ${CC} ${CFLAGS}  ${LDFLAGS} -o pump ${WORKDIR}/pump.c -Wall -lgpiod
}

# Install the executable into the /usr/bin directory on the target
do_install() {
    install -d ${D}${bindir}
    install -m 0755 pump ${D}${bindir}
}
