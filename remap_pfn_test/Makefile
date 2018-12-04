KERNELDIR ?= /lib/modules/$(shell uname -r)/build

# Kernel modules
obj-m := remap_pfn_kmalloc.o
obj-m += remap_pfn_vmalloc.o
obj-m += remap_pfn_alloc_pages.o

# Specify flags for the module compilation.
EXTRA_CFLAGS=-g -O0

build: kernel_modules test_ap kernel_install

kernel_modules:
	make -C $(KERNELDIR) M=$(CURDIR) modules

test_ap:
	arm-linux-gnueabi-gcc remap_pfn_test_1.c -static -o remap_pfn_test_1
	arm-linux-gnueabi-gcc remap_pfn_test_2.c -static -o remap_pfn_test_2
	arm-linux-gnueabi-gcc remap_pfn_test_3.c -static -o remap_pfn_test_3
	arm-linux-gnueabi-gcc remap_pfn_test_4.c -static -o remap_pfn_test_4	
	arm-linux-gnueabi-gcc remap_pfn_test_5.c -static -o remap_pfn_test_5
	arm-linux-gnueabi-gcc remap_pfn_test_6.c dylib.c -static -o remap_pfn_test_6
	
kernel_install:
	cp *.ko remap_pfn_test_1 remap_pfn_test_2 remap_pfn_test_3 remap_pfn_test_4 remap_pfn_test_5 remap_pfn_test_6 $(DRV_INSTALL_DIR)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order
