KERNELDIR ?= /lib/modules/$(shell uname -r)/build

# Kernel modules
obj-m += kobj_test.o

# Specify flags for the module compilation.
EXTRA_CFLAGS=-g -O0

build: kernel_modules kernel_install

kernel_modules:
	make -C $(KERNELDIR) M=$(CURDIR) modules

kernel_install:
	cp *.ko $(DRV_INSTALL_DIR)
	
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order
