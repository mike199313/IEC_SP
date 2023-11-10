FILESEXTRAPATHS:prepend:superion-nuv := "${THISDIR}/${PN}:"

SRC_URI:append:superion-nuv = " file://fw_env.config"

do_install:append:superion-nuv () {
	install -m 644 ${WORKDIR}/fw_env.config ${D}${sysconfdir}/fw_env.config
}
