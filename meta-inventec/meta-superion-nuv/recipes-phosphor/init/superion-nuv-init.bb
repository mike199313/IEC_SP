SUMMARY = "superion-nuv init service"
DESCRIPTION = "Essential init commands for superion-nuv"
PR = "r1"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

inherit obmc-phosphor-systemd

DEPENDS += "systemd"
RDEPENDS:${PN} += "libsystemd"


FILESEXTRAPATHS:prepend := "${THISDIR}/superion-nuv-init:"
SRC_URI += "file://superion-nuv-init.sh \
            file://superion-nuv-gpio-init.sh \
            file://superion-nuv-cpld-init.sh \
            "

S = "${WORKDIR}"

do_install() {
        install -d ${D}${sbindir}
        install -m 0755 superion-nuv-init.sh ${D}${sbindir}
        install -m 0755 superion-nuv-gpio-init.sh ${D}${sbindir}
        install -m 0755 superion-nuv-cpld-init.sh ${D}${sbindir}
}

SYSTEMD_SERVICE:${PN} += "superion-nuv-init.service"

