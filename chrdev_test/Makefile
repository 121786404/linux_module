KERNELDIR ?= /lib/modules/$(shell uname -r)/build

# Kernel modules
obj-m += chardev_test.o

# Specify flags for the module compilation.
EXTRA_CFLAGS=-g -O0

build: kernel_modules kernel_install test_ap

kernel_modules:
	make -C $(KERNELDIR) M=$(CURDIR) modules

kernel_install:
	cp chardev_test_ap *.ko $(DRV_INSTALL_DIR)
	
test_ap:
	arm-linux-gnueabi-gcc chardev_test_ap.c -o chardev_test_ap
	
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order
