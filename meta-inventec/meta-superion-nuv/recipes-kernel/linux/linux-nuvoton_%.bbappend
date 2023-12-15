FILESEXTRAPATHS:prepend:superion-nuv := "${THISDIR}/linux-nuvoton:"

SRC_URI:append:superion-nuv = " \
  file://arch \
  file://drivers \
  file://buv-runbmc.cfg \
  file://0001-Kernel-sync-Intel-BMC-branch-dev-5.15-intel-peci-dri.patch \
  file://0002-Fix-peci-drivers-complier-error.patch \
  file://0003-driver-SPI-add-w25q01jv-support.patch \
  file://0004-Ampere-Altra-MAX-SSIF-IPMI-driver.patch \
  file://0005-driver-misc-seven-segment-display-gpio-driver.patch \
  file://0006-virtual-Add-virtual-driver-config.patch \
  file://0007-inventec-superion-nuv-Modify-TOCK-to-PLL0-for-RGMII-issue.patch \
  file://0008-Add-tmp468-driver.patch \
  file://0009-Modify-sgpio-driver.patch \
  file://0010-Add-Winbond-W25Q512JVFIM.patch \
  file://0011-Add-watchdog-time.patch \
  file://0012-reset-DMA-again-if-stmmac_reset-failed.patch \
  file://0013-Add-cpu-ids-to-support-new-cpu-in-intel-peci-client.patch \
  file://0014-peci-dimmtemp-presence-detection.patch \
  file://0015-kernel-driver-update-for-CPU-SRF-SP.patch \
  file://0016-Add-rom-mx66l2g45g.patch \
  file://0017-Update-xdpe152xx-Family-for-kernel-driver.patch \
  "

# Merge source tree by original project with our layer of additional files
do_add_vesnin_files () {
    cp -r "${WORKDIR}/arch" \
          "${STAGING_KERNEL_DIR}"
    cp -r "${WORKDIR}/drivers" \
          "${STAGING_KERNEL_DIR}"
}
addtask do_add_vesnin_files after do_patch before do_compile

