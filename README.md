# BluePill Arduino Vagrant VM

Vagrantfile for provisioning a VM to build and upload Arduino sketches or libopencm3 code to a STM32 BluePill breakout board.

## Depencies
* VirtualBox
* Vagrant
* vbguest plugin for vagrant.
```
vagrant plugin install vagrant-vbguest
```

## Initial setup
```
user@host$ vagrant up
```

## Build Process
```
user@host$ vagrant ssh
vagrant@stretch$ cd [path to your project]
vagrant@stretch$ make
```
