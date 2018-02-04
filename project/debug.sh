#!/bin/sh
d=$(dirname $0)
cd $d
#screen -c debug.screenrc
arm-none-eabi-gdb -iex "target remote | openocd -c \"gdb_port pipe; log_output openocd.log\" -f debug/bluepill-debug.cfg" blink.ino.elf
