FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
SRC_URI:append = " file://chassis-capabilities.override.yml"
SRC_URI:append = " file://sol-parameters.override.yml"
