#!/bin/bash
mount /dev/sdb /media/usb
rm -f /media/usb/gr01/build/geekos/main.o
rm /media/usb/gr01/build/fd_gr01.img
make -C /media/usb/gr01/build/ & wait $!
umount /dev/sdb & wait $!
