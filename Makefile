SRC_DIR   ?= .
BUILD_DIR ?= build
OUT_DIR   ?= out
LINKER    ?= $(SRC_DIR)/linker.lds

TOOLCHAIN ?= riscv64-elf-

# Required flags, never overridable (changing these breaks the freestanding build).
INCLUDES  := -Iinclude -Iassets
ARCHFLAGS := -ffreestanding -nostdlib -fno-builtin -mabi=ilp32 -march=rv32imzicsr

# Overridable flags (e.g. make CFLAGS="-O0 -g" for a debug build).
CFLAGS    ?= -Wall -O3

# Dependency + map generation — always on, not user-overridable.
DEPFLAGS  := -MMD -MP

SOURCES   := $(shell find $(SRC_DIR) \( -name '*.c' -o -name '*.S' \) \
               ! -path '*/build/*' ! -path '*/out/*' ! -path '*/.cache/*')
OBJECTS   := $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
DEPS      := $(OBJECTS:.o=.d)

C_SOURCES := $(shell find $(SRC_DIR) -name '*.c' \
               ! -path '*/build/*' ! -path '*/out/*' ! -path '*/assets/*')

# Find sources in their subdirs by basename (objects are flattened into build/).
VPATH := $(sort $(dir $(SOURCES)))

.PHONY: all build run clean lint fix fmt size

# Incremental build (only changed files recompile).
all: $(OUT_DIR)/main.bin

# Clean rebuild from scratch.
build: clean all

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(TOOLCHAIN)gcc -c $(ARCHFLAGS) $(CFLAGS) $(INCLUDES) $(DEPFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(@D)
	$(TOOLCHAIN)gcc -c $(ARCHFLAGS) $(CFLAGS) $(INCLUDES) -o $@ $<

$(BUILD_DIR)/main.elf: $(OBJECTS) $(LINKER)
	$(TOOLCHAIN)ld -o $@ -T $(LINKER) -Map=$(BUILD_DIR)/main.map \
	    $(BUILD_DIR)/boot.o $(filter-out $(BUILD_DIR)/boot.o, $(OBJECTS))

$(OUT_DIR)/main.bin: $(BUILD_DIR)/main.elf
	@mkdir -p $(OUT_DIR)
	$(TOOLCHAIN)objcopy --output-target binary $< $@
	$(TOOLCHAIN)objdump -D $< > $(BUILD_DIR)/main.elf.txt

clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)

run: $(OUT_DIR)/main.bin
	dtekv-upload $< 0x00000000

size: $(BUILD_DIR)/main.elf
	$(TOOLCHAIN)size $<

# Pull in generated header dependencies (silently ignored on first build).
-include $(DEPS)
