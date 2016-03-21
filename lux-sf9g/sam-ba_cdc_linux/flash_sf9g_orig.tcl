DATAFLASH::Init 1
DATAFLASH::EraseAll
GENERIC::SendBootFile ../images/dataflash_at91sam9g20ek.bin
send_file {DataFlash AT45DB/DCB} ../images/u-boot-1.3.4-exp.4-at91sam9g20ek-dataflash_cs1.bin 0x8400 0
