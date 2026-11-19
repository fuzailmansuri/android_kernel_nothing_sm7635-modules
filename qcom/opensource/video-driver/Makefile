# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(TARGET_BOARD_PLATFORM),)
override TARGET_BOARD_PLATFORM := volcano
endif
ifeq ($(BOARD_PLATFORM),)
override BOARD_PLATFORM := $(TARGET_BOARD_PLATFORM)
endif

VIDEO_ROOT := $(KERNEL_SRC)/$(M)
KBUILD_OPTIONS += VIDEO_ROOT=$(VIDEO_ROOT)
KBUILD_OPTIONS += BOARD_PLATFORM=$(BOARD_PLATFORM)
KBUILD_OPTIONS += TARGET_BOARD_PLATFORM=$(TARGET_BOARD_PLATFORM)

all: modules

modules:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) modules $(KBUILD_OPTIONS)

modules_install:
	$(MAKE) INSTALL_MOD_STRIP=1 -C $(KERNEL_SRC) M=$(M) modules_install

%:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) $@ $(KBUILD_OPTIONS)

clean:
	rm -f *.o *.ko *.mod.c *.mod.o *~ .*.cmd Module.symvers
	rm -rf .tmp_versions
