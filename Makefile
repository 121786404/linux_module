export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-
export KERNELDIR=~/linux-qemu-lab-arm/linux
export DRV_INSTALL_DIR=~/linux-qemu-lab-arm/root_nfs/share


EXCLUDE_DIRS := include bin
SUBDIRS := $(shell find . -maxdepth 1 -type d -mtime -1)
SUBDIRS := $(basename $(patsubst ./%,%,$(SUBDIRS)))
SUBDIRS := $(filter-out $(EXCLUDE_DIRS),$(SUBDIRS))


all: subdirs

subdirs:
	#echo ${SUBDIRS}
	rm -f $(DRV_INSTALL_DIR)/*.ko
	for n in $(SUBDIRS); do $(MAKE) -C $$n || exit 1; done

clean:
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done

