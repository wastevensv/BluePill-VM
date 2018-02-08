# BluePill Arduino Vagrant VM

Vagrantfile for provisioning a VM to build and upload Arduino sketches to a STM32 BluePill breakout board.

## Depencies
* VirtualBox
* Vagrant
* vbguest plugin for vagrant.
```
vagrant plugin install vagrant-vbguest
```

## Initial setup
```
vagrant up
```

## Build and Upload Process
```
vagrant ssh -c "cd bluepill && make upload"
```
