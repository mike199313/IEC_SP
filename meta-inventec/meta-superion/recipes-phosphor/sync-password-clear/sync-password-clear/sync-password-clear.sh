#!/bin/bash

SYNC_PER_SEC=5

# This script is in order to sync the value of vGPIO4 and PASSWORD_CLEAR_N continuously.
while true
do
    PASSWORD_CLEAR=`gpioget $(gpiofind PASSWORD_CLEAR_N)`
    inventec-vgpio -s $PASSWORD_CLEAR -n FM_PASSWORD_CLEAR_N
    sleep $SYNC_PER_SEC
done
