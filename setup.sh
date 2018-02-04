#!/bin/bash
LAUNCH_USER="${LAUNCH_USER:-vagrant}"
rm -rfv /tmp/setup-downloads

sudo apt-get install -y unzip usbutils minicom gcc-arm-none-eabi gdb-arm-none-eabi libc6-i386 openocd screen

mkdir -v /tmp/setup-downloads
cd /tmp/setup-downloads

wget -q https://github.com/arduino/arduino-builder/releases/download/1.3.25/arduino-builder-linux64-1.3.25.tar.bz2 -O arduino-builder.tar.bz2
wget -q http://dan.drown.org/stm32duino/tools_linux64-2018.2.2.tar.gz -O bp-tools.tar.gz
wget -q http://dan.drown.org/stm32duino/STM32F1-2018.2.2.zip -O bp-hw.zip

rm -rfv /home/$LAUNCH_USER/.bluepill-env
mkdir -pv /home/$LAUNCH_USER/.bluepill-env/{hardware,packages/stm32duino/hardware,tools}
cd /home/$LAUNCH_USER/.bluepill-env

tar xvf /tmp/setup-downloads/arduino-builder.tar.bz2
cd packages/stm32duino
tar xvf /tmp/setup-downloads/bp-tools.tar.gz
cd hardware
unzip /tmp/setup-downloads/bp-hw.zip
cd /home/$LAUNCH_USER/.bluepill-env

mv -v /tmp/udev-rules/* /etc/udev/rules.d
rmdir /tmp/udev-rules/
udevadm control --reload-rules

usermod -aG dialout,plugdev $LAUNCH_USER
