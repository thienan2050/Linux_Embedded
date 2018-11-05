This is an example about Character Device Drivers.
How to run it:

Download source file and Makefile into your working directory.
Run: make. You can see *.ko file after compiling
Run: dmesg -C to clear all messages.
Run: insmod *.ko to add *.ko to kernel. You can see module *.ko in lsmod
Type: echo "out" > /dev/LED_Driver to set pin 17 output
Type: echo "on" > /dev/LED_Driver to set pin 17 on
Type: echo "off" > /dev/LED_Driver to set pin 17 off.
Run: dmesg to see all messages
