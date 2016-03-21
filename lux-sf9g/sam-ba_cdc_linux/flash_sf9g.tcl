DATAFLASH::Init 1
DATAFLASH::EraseAll
GENERIC::SendBootFile ../images/boot-lux-sf9g-dataflash2sdram.bin
send_file {DataFlash AT45DB/DCB} ../images/u-boot.bin 0x8400 0
