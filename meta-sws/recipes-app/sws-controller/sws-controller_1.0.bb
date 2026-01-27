SUMMARY = "Smart Watering System Controller"
DESCRIPTION = "Main control loop for ADC-based moisture sensor and pump"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://sws_controller.c"

S = "${WORKDIR}"

DEPENDS = ""

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} sws_controller.c -o sws_controller
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 sws_controller ${D}${bindir}/sws_controller
}

