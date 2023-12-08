FILESEXTRAPATHS:prepend:superion-dvt := "${THISDIR}/linux-aspeed:"

SRC_URI:append:superion-dvt = " file://superion-dvt.cfg \
                                file://arch \
                              "
