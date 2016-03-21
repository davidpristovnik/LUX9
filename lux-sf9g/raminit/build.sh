#!/bin/bash

#make CROSS=arm-none-eabi- CHIP=at91sam9260 BOARD=at91sam9260-ek MEMORIES=sram
make clean
# make address bus test
make CROSS=arm-none-eabi- CHIP=at91sam9g20 BOARD=at91sam9g20-ek MEMORIES=sram TESTTYPE=address
# make data bus test
make CROSS=arm-none-eabi- CHIP=at91sam9g20 BOARD=at91sam9g20-ek MEMORIES=sram TESTTYPE=data
# make serial output test
make CROSS=arm-none-eabi- CHIP=at91sam9g20 BOARD=at91sam9g20-ek MEMORIES=sram TESTTYPE=serial

cp bin/*.bin ../QCmemtest/images/
cp bin/AC-memtest-data.bin /home/davidp/devel/lux9g20/sam-ba_cdc_linux/tcl_lib/at91sam9g20-ek/isp-extram-at91sam9g20.bin
#cp bin/AC-memtest-address.bin /home/davidp/devel/lux9g20/sam-ba_cdc_linux/tcl_lib/at91sam9g20-ek/isp-extram-at91sam9g20.bin
