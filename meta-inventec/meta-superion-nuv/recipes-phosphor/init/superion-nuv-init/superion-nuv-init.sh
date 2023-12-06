#!/bin/sh


# Init GPIO setting
GPIO_INIT_SH="/usr/sbin/superion-nuv-gpio-init.sh"
bash $GPIO_INIT_SH

# Read cpld verion
CPLD_CHECK="/usr/sbin/superion-nuv-cpld-init.sh"
bash $CPLD_CHECK

# Init Fan setting
#FAN_INIT_SH="/usr/sbin/superion-nuv-fan-init.sh"
#bash $FAN_INIT_SH
