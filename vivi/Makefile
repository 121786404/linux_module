KERNELDIR ?= /lib/modules/$(shell uname -r)/build

# Kernel modules
obj-m += myvivi.o

# Specify flags for the module compilation.
EXTRA_CFLAGS=-g -O0

build: kernel_modules v4l2_ap kernel_install

kernel_modules:
	make -C $(KERNELDIR) M=$(CURDIR) modules

v4l2_ap:
	arm-linux-gnueabi-gcc v4l2_test.c -o v4l2_test
	
kernel_install:
	cp *.ko v4l2_test $(DRV_INSTALL_DIR)
	
	
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order v4l2_test
