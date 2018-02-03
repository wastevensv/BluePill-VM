# BluePill Arduino Vagrant VM

Vagrantfile for provisioning a VM to build and upload Arduino sketches to a STM32 BluePill breakout board.

## Initial setup
```
vagrant up
```

## Build and Upload Process
```
vagrant ssh -c "cd bluepill && make upload"
```
