KERNELDIR ?= /lib/modules/$(shell uname -r)/build

# Kernel modules
obj-m += cdata_test.o

# Specify flags for the module compilation.
EXTRA_CFLAGS=-g -O0

build: kernel_modules test_ap kernel_install

kernel_modules:
	make -C $(KERNELDIR) M=$(CURDIR) modules

kernel_install:
	cp cdata_test_ap *.ko $(DRV_INSTALL_DIR)
	
test_ap:
	arm-linux-gnueabi-gcc cdata_test_ap.c -o cdata_test_ap
	
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order cdata_test_ap
