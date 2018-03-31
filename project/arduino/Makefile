TARGET:=blink
BUILDER_DIR=~/.bluepill-env
BUILDER_BIN=$(BUILDER_DIR)/arduino-builder
TOOL_DIR=$(BUILDER_DIR)/packages/stm32duino/tools/linux

OPENOCD_BIN=openocd
DEBUG_CFG_DIR=./debug
BOOTLOADER_BIN=$(DEBUG_CFG_DIR)/generic_boot20_pc13.bin

BUILDER_FLAGS=
ifdef DEBUG
BUILDER_FLAGS+=-verbose
endif

GDB_BIN=arm-none-eabi-gdb

.SILENT:

.PHONY: build upload bootload program debug

build: $(TARGET).bin

flash: $(TARGET).bin
	echo "Uploading to board..."
	echo "Push reset."
	sleep 2
	$(TOOL_DIR)/dfu-util/dfu-util -d 1EAF:0003 -a 2 -D $< -R

$(TARGET).bin: $(TARGET)/main.ino
	echo "Building $<..."
	mkdir -pv /tmp/$(TARGET)-build/
	$(BUILDER_BIN) $(BUILDER_FLAGS) -build-path /tmp/$(TARGET)-build/ -build-options-file build.options.json $<
	cp -v /tmp/$(TARGET)-build/main.ino.bin $(TARGET).bin
	cp -v /tmp/$(TARGET)-build/main.ino.elf $(TARGET).elf

bootload: $(BOOTLOADER_BIN)
program:  $(TARGET).bin

bootload program:
	echo "Flashing $?..."
	cp $< firmware.bin
	$(OPENOCD_BIN) -f $(DEBUG_CFG_DIR)/bluepill-reset.cfg
	rm firmware.bin

clean:
	-rm -v *.bin *.elf
