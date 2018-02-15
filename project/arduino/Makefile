TARGET=blink.ino
BUILDER_DIR=~/.bluepill-env
BUILDER_BIN=$(BUILDER_DIR)/arduino-builder
TOOL_DIR=$(BUILDER_DIR)/packages/stm32duino/tools/linux

OPENOCD_BIN=openocd
DEBUG_CFG_DIR=./debug
BOOTLOADER_BIN=$(DEBUG_CFG_DIR)/generic_boot20_pc13.bin

GDB_BIN=arm-none-eabi-gdb

.SILENT:

.PHONY: build upload bootload program debug

build: $(TARGET).bin

upload: $(TARGET).bin
	echo "Uploading to board..."
	$(TOOL_DIR)/upload-reset /dev/ttyACM0 1500
	$(TOOL_DIR)/dfu-util/dfu-util -d 1EAF:0003 -a 2 -D $< -R

$(TARGET).bin: $(TARGET)
	echo "Building $<..."
	mkdir -pv /tmp/$(TARGET)-build/
	$(BUILDER_BIN) -build-path /tmp/$(TARGET)-build/ -build-options-file build.options.json $<
	cp -v /tmp/$(TARGET)-build/$(TARGET).bin .
	cp -v /tmp/$(TARGET)-build/$(TARGET).elf .

bootload: $(BOOTLOADER_BIN)
program:  $(TARGET).bin

bootload program:
	echo "Flashing $?..."
	cp $< firmware.bin
	$(OPENOCD_BIN) -f $(DEBUG_CFG_DIR)/bluepill-reset.cfg
	rm firmware.bin

