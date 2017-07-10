export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-
export KERNELDIR=~/linux-qemu-lab-arm/linux
export DRV_INSTALL_DIR=~/linux-qemu-lab-arm/root_nfs/share


EXCLUDE_DIRS := include bin
#SUBDIRS := $(shell find . -path './bak' -prune -o -type d -mtime -1 -maxdepth 1 -print)
SUBDIRS := $(shell find . -path './bak' -prune -o -type d -maxdepth 1 -print)
SUBDIRS := $(basename $(patsubst ./%,%,$(SUBDIRS)))
SUBDIRS := $(filter-out $(EXCLUDE_DIRS),$(SUBDIRS))




all: subdirs

subdirs:
	mkdir -p $(DRV_INSTALL_DIR)
	rm -rf $(DRV_INSTALL_DIR)/*.ko
	for n in $(SUBDIRS); do $(MAKE) -C $$n || exit 1; done

clean:
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done

