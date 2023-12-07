SUMMARY = "Synchronize the value of vGPIO4 and PASSWORD_CLEAR_N"
DESCRIPTION = "This script is in order to synchronize the value of vGPIO4 and PASSWORD_CLEAR_N continuously."
PR = "r1"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

inherit obmc-phosphor-systemd

DEPENDS += "systemd"
RDEPENDS:${PN} += "libsystemd bash"

FILESEXTRAPATHS:prepend := "${THISDIR}/sync-password-clear:"

FILES:${PN}:append = " ${sbindir}/sync-password-clear.sh"

SRC_URI = "\
            file://sync-password-clear.sh \
          "

S = "${WORKDIR}"

do_install() {
        install -d ${D}${sbindir}
        install -m 0755 sync-password-clear.sh ${D}${sbindir}
}

SYSTEMD_SERVICE:${PN} += "sync-password-clear.service"
