PREFIX = arm-none-eabi-
CPP = g++
CC = gcc
LD = gcc
OBJDUMP = objdump
OBJCOPY = objcopy

OBJS            += src/cdcacm.o

DEVICE = stm32f103c8
LIBNAME = opencm3_stm32f1

OPENCM3_DIR     = /home/`whoami`/.bluepill-env/libopencm3

DEFS		+= -DSTM32F1
DEFS		+= -I$(OPENCM3_DIR)/include

FP_FLAGS	?= -msoft-float
ARCH_FLAGS = -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd

CFLAGS          += -Os -ggdb3 -std=c99 $(DEFS) $(ARCH_FLAGS)
CPPFLAGS	+= -MD $(DEFS) $(ARCH_FLAGS)

LDSCRIPT 	= ../bluepill.ld

LDFLAGS         += -static -nostartfiles
LDFLAGS		+= -L$(OPENCM3_DIR)/lib -T$(LDSCRIPT)
LDFLAGS		+= $(ARCH_FLAGS)

LDLIBS	 	+= -Wl,-lc -Wl,-lgcc -Wl,-lnosys -Wl,-l$(LIBNAME)

MAKEFLAGS += --no-print-directory

.SILENT:
.PHONY: clean all libopencm3 flash

all: binary.elf binary.bin

%.bin: %.elf
	printf "  OBJCOPY $(*).bin\n"
	$(PREFIX)$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.list: %.elf
	printf "  OBJDUMP $(*).list\n"
	$(PREFIX)$(OBJDUMP) -S $(*).elf > $(*).list

%.elf %.map: $(OBJS) $(LDSCRIPT)
	printf "  LD      $(*).elf\n"
	$(PREFIX)$(LD) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(*).elf

%.o: %.c libopencm3
	printf "  CC      $(*).c\n"
	$(PREFIX)$(CC) $(CFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).c

%.o: %.cpp libopencm3
	printf "  CXX     $(*).cpp\n"
	$(PREFIX)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cpp

clean:
	printf "  CLEAN\n"
	$(RM) *.o *.d *.elf *.bin *.hex *.srec *.list *.map generated.* ${OBJS} ${OBJS:%.o:%.d}

libopencm3:
	$(MAKE) -C $(OPENCM3_DIR) --no-print-directory

flash: binary.bin
	echo "Uploading to board..."
	echo "Push reset."
	sleep 2
	dfu-util -d 1EAF:0003 -a 2 -D $< -R
