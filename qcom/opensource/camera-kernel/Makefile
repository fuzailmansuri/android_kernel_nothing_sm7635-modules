# Makefile for use with Android's kernel/build system

ifeq ($(TARGET_BOARD_PLATFORM),)
override TARGET_BOARD_PLATFORM := volcano
endif
ifeq ($(BOARD_PLATFORM),)
override BOARD_PLATFORM := $(TARGET_BOARD_PLATFORM)
endif

KBUILD_OPTIONS += CAMERA_KERNEL_ROOT=$(shell pwd)
KBUILD_OPTIONS += KERNEL_ROOT=$(ROOT_DIR)/$(KERNEL_DIR)
KBUILD_OPTIONS += MODNAME=camera
KBUILD_OPTIONS += BOARD_PLATFORM=$(BOARD_PLATFORM)
KBUILD_OPTIONS += TARGET_BOARD_PLATFORM=$(TARGET_BOARD_PLATFORM)

KBUILD_OPTIONS += KBUILD_EXTRA_SYMBOLS+=$(M)/../securemsm-kernel/Module.symvers

all: modules

modules dtbs:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) modules $(KBUILD_OPTIONS)

modules_install:
	$(MAKE) M=$(M) -C $(KERNEL_SRC) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) clean
