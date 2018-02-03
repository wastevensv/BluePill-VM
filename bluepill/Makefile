TARGET=blink
BUILDER_DIR=~/.bluepill-env
BUILDER_BIN=$(BUILDER_DIR)/arduino-builder
TOOL_DIR=$(BUILDER_DIR)/packages/stm32duino/tools/linux
UPLOAD_BIN=$(TOOL_DIR)/maple_upload

.PHONY: build upload

build: $(TARGET).ino.bin

$(TARGET).ino.bin: $(TARGET).ino
	mkdir -pv /tmp/$(TARGET)-build/
	$(BUILDER_BIN) -verbose -build-path /tmp/$(TARGET)-build/ -build-options-file build.options.json $<
	mv -v /tmp/$(TARGET)-build/$(TARGET).ino.bin .

upload: $(TARGET).ino.bin
	$(TOOL_DIR)/upload-reset /dev/ttyACM0 1500
	$(TOOL_DIR)/dfu-util/dfu-util -d 1EAF:0003 -a 2 -D $(TARGET).ino.bin -R
