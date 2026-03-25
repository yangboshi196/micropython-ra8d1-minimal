include ../../py/mkenv.mk

# 1. 基础配置
CROSS = 1
CROSS_COMPILE ?= arm-none-eabi-
LD = $(CROSS_COMPILE)gcc

QSTR_DEFS = qstrdefsport.h
MICROPY_ROM_TEXT_COMPRESSION ?= 1

include $(TOP)/py/py.mk

# 2. 路径定义
FSP_DIR = $(TOP)/lib/renesas_ra

# 3. 头文件搜索路径 
INC += -I. -I$(TOP) -I$(BUILD)
INC += -I$(FSP_DIR)/ra_gen
INC += -I$(FSP_DIR)/ra_cfg/fsp_cfg
INC += -I$(FSP_DIR)/ra_cfg/fsp_cfg/bsp
INC += -I$(FSP_DIR)/ra/fsp/inc
INC += -I$(FSP_DIR)/ra/fsp/inc/api
INC += -I$(FSP_DIR)/ra/fsp/inc/instances
INC += -I$(FSP_DIR)/ra/board/ra8d1_aik
INC += -I$(FSP_DIR)/ra/arm/CMSIS_6/CMSIS/Core/Include
INC += -I$(FSP_DIR)/ra/fsp/src/bsp/cmsis/Device/RENESAS/Include
INC += -I$(FSP_DIR)/ra/fsp/src/bsp/mcu/all
INC += -I$(FSP_DIR)/ra/fsp/src/bsp/mcu/ra8d1

# 4. 编译器参数 (M85 核心指令)
CFLAGS_CORTEX_M85 = -mthumb -mcpu=cortex-m85 -mfloat-abi=hard -mfpu=fpv5-d16
CFLAGS += $(INC) -Wall -Werror -std=c99 -nostdlib $(CFLAGS_CORTEX_M85) $(COPT)

# 5. 链接器参数
LDFLAGS += -nostdlib -T fsp.ld -Wl,-Map=$@.map -Wl,--cref -Wl,--gc-sections
LDFLAGS += $(CFLAGS_CORTEX_M85) --specs=nosys.specs

# 优化设置
CFLAGS += -g
ifeq ($(DEBUG), 1)
CFLAGS += -O0
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# 6. 源码抓取 (FSP 部分)
SRC_FSP_C = $(wildcard $(FSP_DIR)/ra_gen/*.c) \
            $(wildcard $(FSP_DIR)/ra/fsp/src/bsp/mcu/all/*.c) \
            $(wildcard $(FSP_DIR)/ra/fsp/src/bsp/mcu/ra8d1/*.c) \
            $(wildcard $(FSP_DIR)/ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/*.c) \
            $(wildcard $(FSP_DIR)/ra/fsp/src/r_ioport/*.c) \
            $(wildcard $(FSP_DIR)/ra/fsp/src/r_sci_b_uart/*.c) \
            $(wildcard $(FSP_DIR)/ra/board/ra8d1_aik/*.c)

# 7. 最终源码缝合
SRC_C = main.c \
        uart_core.c \
        shared/libc/printf.c \
        shared/readline/readline.c \
        shared/runtime/pyexec.c \
        shared/runtime/stdout_helpers.c \
        $(BUILD)/_frozen_mpy.c \
        $(SRC_FSP_C)

SRC_C += shared/libc/string0.c
SRC_QSTR += shared/readline/readline.c shared/runtime/pyexec.c

OBJ += $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

all: $(BUILD)/firmware.elf

$(BUILD)/_frozen_mpy.c: $(TOP)/tests/frozen/frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(TOP)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

include $(TOP)/py/mkrules.mk