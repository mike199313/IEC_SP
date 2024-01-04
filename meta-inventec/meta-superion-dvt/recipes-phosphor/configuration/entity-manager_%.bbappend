FILESEXTRAPATHS:prepend:${MACHINE} := "${THISDIR}/${PN}:"

SRC_URI += " \
  file://motherboard_dvt.json \
  file://blacklist_dvt.json \
  file://runbmc_dvt.json \
  file://scmbridge_dvt.json \
  file://bp0_dvt.json \
  file://bp1_dvt.json \
  file://artesyn_csu2400ap-3-100_psu1_dvt.json \
  file://artesyn_csu2400ap-3-100_psu2_dvt.json \
"

do_install:append:${MACHINE} () {
  install -d 0755 ${D}${datadir}/${PN}/configurations/
  rm -v -f ${D}${datadir}/${PN}/configurations/*.json
  install -m 0644 ${WORKDIR}/motherboard_dvt.json ${D}${datadir}/${PN}/configurations/motherboard.json
  install -m 0644 ${WORKDIR}/blacklist_dvt.json ${D}${datadir}/${PN}/blacklist.json
  install -m 0644 ${WORKDIR}/runbmc_dvt.json ${D}${datadir}/${PN}/configurations/runbmc.json
  install -m 0644 ${WORKDIR}/scmbridge_dvt.json ${D}${datadir}/${PN}/configurations/scmbridge.json
  install -m 0644 ${WORKDIR}/bp0_dvt.json ${D}${datadir}/${PN}/configurations/bp0.json
  install -m 0644 ${WORKDIR}/bp1_dvt.json ${D}${datadir}/${PN}/configurations/bp1.json
  install -m 0644 ${WORKDIR}/artesyn_csu2400ap-3-100_psu1_dvt.json ${D}${datadir}/${PN}/configurations/artesyn_csu2400ap-3-100_psu1.json
  install -m 0644 ${WORKDIR}/artesyn_csu2400ap-3-100_psu2_dvt.json ${D}${datadir}/${PN}/configurations/artesyn_csu2400ap-3-100_psu2.json
}
