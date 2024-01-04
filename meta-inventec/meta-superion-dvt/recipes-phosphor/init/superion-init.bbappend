
FILESEXTRAPATHS:prepend := "${THISDIR}/superion-init:"

S = "${WORKDIR}"

do_install:append() {
        # fan bus has been changed from i2c-21 to i2c-25 in the DVT layout update
        sed -i -e 's,FAN_BUS_NUM=21,FAN_BUS_NUM=25,g' ${D}${sbindir}/superion-fan-init.sh
}
