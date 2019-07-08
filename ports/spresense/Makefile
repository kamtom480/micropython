include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

CROSS_COMPILE ?= arm-none-eabi-

SPRESENSE_SDK = spresense-sdk

INC += -I.
INC += -I$(TOP)
INC += -I$(TOP)/lib/timeutils
INC += -I$(BUILD)
INC += -I$(SPRESENSE_SDK)/nuttx/include
INC += -I$(SPRESENSE_SDK)/nuttx/arch
INC += -I$(SPRESENSE_SDK)/nuttx/arch/chip
INC += -I$(SPRESENSE_SDK)/sdk/bsp/include
INC += -I$(SPRESENSE_SDK)/sdk/bsp/include/sdk

CFLAGS = $(INC) -DCONFIG_WCHAR_BUILTIN -DCONFIG_HAVE_DOUBLE -c -w -std=gnu11 -c  \
	-fno-builtin -mabi=aapcs -Wall -Os -fno-strict-aliasing -fno-strength-reduce \
	-fomit-frame-pointer -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
	-pipe -ggdb -gdwarf-3 -ffunction-sections -fdata-sections -DF_CPU=156000000L
LDFLAGS = -ggdb -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
	-Xlinker --entry=__start -nodefaultlibs -nostartfiles -Wl,--defsym,__stack=_vectors+786432 \
	-T$(SPRESENSE_SDK)/nuttx/build/ramconfig.ld -Wl,--gc-sections -Xlinker -Map=$(BUILD)/output.map \
	-o $(BUILD)/micropython.elf -Wl,--start-group -u spresense_main  \
	$(BUILD)/libmpy.a $(SPRESENSE_SDK)/sdk/libs/libapps.a $(SPRESENSE_SDK)/sdk/libs/libsdk.a \
	-lgcc -lm -Wl,--end-group -L$(BUILD)
LIBS =

SRC_C = \
	main.c \
	mphalport.c \
	modmachine.c \
	machine_pin.c \
	machine_spi.c \
	machine_i2c.c \
	machine_uart.c \
	machine_rtc.c \
	machine_adc.c \
	machine_pwm.c \
	modutime.c \
	moduos.c \
	file.c \
	lib/utils/printf.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	lib/timeutils/timeutils.c \
	drivers/bus/softspi.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C)
# Append any auto-generated sources that are needed by sources listed in SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

all: $(BUILD)/micropython.spk

$(SPRESENSE_SDK):
	$(ECHO) ""
	$(ECHO) "Please run the following command to clone the Spresense SDK:"
	$(ECHO) "git clone https://github.com/sonydevworld/spresense-micropython.git spresense-sdk"
	$(ECHO) ""
	$(ECHO) "run make again to create the micropython.spk image."
	exit 1

$(SPRESENSE_SDK)/firmware:
	$(ECHO) ""
	$(ECHO) "Download the spresense binaries zip archive from:"
	$(ECHO) "https://developer.sony.com/file/download/download-spresense-firmware-v1-3-000"
	$(ECHO) ""
	$(ECHO) "run make flash-bootloader again to flash bootloader."
	exit 1

$(BUILD)/libmpy.a: $(SPRESENSE_SDK) $(OBJ) 
	$(ECHO) "AR $@"
	$(Q)$(AR) rcs $(BUILD)/libmpy.a $(OBJ)

$(BUILD)/micropython.elf: $(BUILD)/libmpy.a
	$(ECHO) "CXX $@"
	$(Q)$(CXX) $(LDFLAGS)

$(BUILD)/micropython.spk: $(BUILD)/micropython.elf
	$(ECHO) "Creating $@"
	$(SPRESENSE_SDK)/sdk/tools/linux/mkspk -c 2 $(BUILD)/micropython.elf nuttx $(BUILD)/micropython.spk

flash: $(BUILD)/micropython.spk
	$(ECHO) "Writing $< to the board"
	$(SPRESENSE_SDK)/sdk/tools/flash.sh -c /dev/ttyUSB0 $(BUILD)/micropython.spk

flash-bootloader: $(SPRESENSE_SDK) $(SPRESENSE_SDK)/firmware
	$(ECHO) "Writing loader to the board"
	$(SPRESENSE_SDK)/sdk/tools/flash.sh -l firmware/ -c /dev/ttyUSB0

include $(TOP)/py/mkrules.mk
