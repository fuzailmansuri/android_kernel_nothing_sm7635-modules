# SPDX-License-Identifier: GPL-2.0

ifeq ($(TARGET_BOARD_PLATFORM),)
override TARGET_BOARD_PLATFORM := volcano
endif
ifeq ($(BOARD_PLATFORM),)
override BOARD_PLATFORM := $(TARGET_BOARD_PLATFORM)
endif

KBUILD_OPTIONS+= DISPLAY_ROOT=$(KERNEL_SRC)/$(M)/../
KBUILD_OPTIONS += BOARD_PLATFORM=$(BOARD_PLATFORM)
KBUILD_OPTIONS += TARGET_BOARD_PLATFORM=$(TARGET_BOARD_PLATFORM)
KBUILD_OPTIONS += KBUILD_EXTRA_SYMBOLS+=$(M)/../../mm-drivers/msm_ext_display/Module.symvers
KBUILD_OPTIONS += KBUILD_EXTRA_SYMBOLS+=$(M)/../../mm-drivers/hw_fence/Module.symvers
KBUILD_OPTIONS += KBUILD_EXTRA_SYMBOLS+=$(M)/../../mm-drivers/sync_fence/Module.symvers
KBUILD_OPTIONS += KBUILD_EXTRA_SYMBOLS+=$(M)/../../securemsm-kernel/Module.symvers

all: modules

modules_install:
	$(MAKE) INSTALL_MOD_STRIP=1 -C $(KERNEL_SRC) M=$(M) modules_install

%:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) $@ $(KBUILD_OPTIONS)

clean:
	rm -f *.o *.ko *.mod.c *.mod.o *~ .*.cmd Module.symvers
	rm -rf .tmp_versions

