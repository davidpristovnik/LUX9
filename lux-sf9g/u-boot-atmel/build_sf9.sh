#!/bin/bash
CROSS_COMPILE=arm-none-linux-gnueabi- make lux_sf9g_nandflash
cp u-boot.bin ../../images
