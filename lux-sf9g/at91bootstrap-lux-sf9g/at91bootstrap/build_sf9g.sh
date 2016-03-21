#!/bin/bash                                                                                                                                                                                                                                 
make CHIP=at91sam9g20 BOARD=lux-sf9g ORIGIN=dataflash SLOT=SLOT_B DESTINATION=sdram BIN_SIZE=0x35000 FROM_ADDR=0x8400 DEST_ADDR=0x23F00000 OP_BOOTSTRAP=on STR_DESCR=\\\"u-boot\\\" TRACE_LEVEL=1 clean all
cp bin/boot-lux-sf9g-dataflash2sdram.bin ../../images
