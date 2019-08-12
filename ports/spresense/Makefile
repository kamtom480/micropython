include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

CROSS_COMPILE ?= arm-none-eabi-

SPRESENSE_SDK = spresense-sdk

FIRMWARE = $(SPRESENSE_SDK)/firmware

INC = \
	-I. \
	-I$(TOP) \
	-I$(TOP)/lib/timeutils \
	-I$(BUILD) \
	-I$(SPRESENSE_SDK)/nuttx/include \
	-I$(SPRESENSE_SDK)/nuttx/arch \
	-I$(SPRESENSE_SDK)/nuttx/arch/chip \
	-I$(SPRESENSE_SDK)/sdk/bsp/include \
	-I$(SPRESENSE_SDK)/sdk/bsp/include/sdk \

CFLAGS = \
	$(INC) \
	-DCONFIG_WCHAR_BUILTIN \
	-DCONFIG_HAVE_DOUBLE \
	-c \
	-w \
	-Os \
	-pipe \
	-std=gnu11 \
	-mcpu=cortex-m4 \
	-mthumb \
	-mfpu=fpv4-sp-d16 \
	-mfloat-abi=hard \
	-mabi=aapcs \
	-fno-builtin \
	-fno-strict-aliasing \
	-fno-strength-reduce \
	-fomit-frame-pointer \
	-ffunction-sections \
	-fdata-sections \
	-Wall \

LIBM = "${shell "$(CC)" $(CFLAGS) -print-file-name=libm.a}"

LIBGCC = "${shell "$(CC)" $(CFLAGS) -print-libgcc-file-name}"

LDFLAGS = \
	--entry=__start \
	-nostartfiles \
	-nodefaultlibs \
	--defsym __stack=_vectors+786432 \
	-T$(SPRESENSE_SDK)/nuttx/build/ramconfig.ld \
	--gc-sections \
	-Map=$(BUILD)/output.map \
	-o $(BUILD)/micropython.elf \
	--start-group \
	-u spresense_main \
	$(BUILD)/libmpy.a \
	$(SPRESENSE_SDK)/sdk/libs/libapps.a \
	$(SPRESENSE_SDK)/sdk/libs/libsdk.a \
	$(LIBM) \
	$(LIBGCC) \
	--end-group \
	-L$(BUILD) \

SRC_C = \
	main.c \
	mphalport.c \
	modmachine.c \
	machine_pin.c \
	machine_spi.c \
	machine_i2c.c \
	machine_uart.c \
	lib/utils/printf.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
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

$(FIRMWARE):
	$(ECHO) ""
	$(ECHO) "Download the spresense binaries zip archive from:"
	$(ECHO) "https://developer.sony.com/file/download/download-spresense-firmware-v1-3-000"
	$(ECHO) "Extract spresense binaries to $(FIRMWARE)"
	$(ECHO) ""
	$(ECHO) "run make flash-bootloader again to flash bootloader."
	exit 1

$(BUILD)/libmpy.a: $(SPRESENSE_SDK) $(OBJ) 
	$(ECHO) "AR $@"
	$(Q)$(AR) rcs $(BUILD)/libmpy.a $(OBJ)

$(BUILD)/micropython.elf: $(BUILD)/libmpy.a
	$(ECHO) "LD $@"
	$(Q)$(LD) $(LDFLAGS)

$(BUILD)/micropython.spk: $(BUILD)/micropython.elf
	$(ECHO) "Creating $@"
	$(SPRESENSE_SDK)/sdk/tools/linux/mkspk -c 2 $(BUILD)/micropython.elf nuttx $(BUILD)/micropython.spk

flash: $(BUILD)/micropython.spk
	$(ECHO) "Writing $< to the board"
	$(SPRESENSE_SDK)/sdk/tools/flash.sh -c /dev/ttyUSB0 $(BUILD)/micropython.spk

flash-bootloader: $(SPRESENSE_SDK) $(FIRMWARE)
	$(ECHO) "Writing loader to the board"
	$(SPRESENSE_SDK)/sdk/tools/flash.sh -l $(FIRMWARE) -c /dev/ttyUSB0

include $(TOP)/py/mkrules.mk
