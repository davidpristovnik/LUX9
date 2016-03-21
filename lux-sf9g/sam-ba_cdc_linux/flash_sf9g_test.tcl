DATAFLASH::Init 1
DATAFLASH::EraseAll
GENERIC::SendBootFile ../test/boot-lux-sf9g-dataflash2sdram.bin
send_file {DataFlash AT45DB/DCB} ../test/u-boot.bin 0x8400 0
