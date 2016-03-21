CROSS_COMPILE=arm-none-linux-gnueabi-
MODULE_NAME=at91_adc
KVER=2.6.38.7
MODDESTDIR=../modules/lib/modules/$(KVER)/kernel/drivers/misc/at91_adc

ifneq ($(KERNELRELEASE),)
obj-m := at91_adc.o

else
PWD  := $(shell pwd)
KDIR ?= $(PWD)/../linux-2.6.38.7/

all: modules install at91_adc_test

modules:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers
	rm -f at91_adc_test

.PHONY: modules clean

install:
	mkdir -p ../modules/lib/modules/$(KVER)/kernel/drivers/misc/at91_adc
	install -p -m 644 $(MODULE_NAME).ko  $(MODDESTDIR)

endif

at91_adc_test:
	$(CROSS_COMPILE)gcc -o at91_adc_test at91_adc_test.c

