This is an example about Linux Embedded.
To run the demo, try the following:
+ Run [dmesg] to clear all messages. 
+ Download source file and Makefile into your working directory.
+ Run [make]. After compiling, you can see *.ko file.
+ Run [insmod *.ko] to add *.ko into Kernel.
+ To set pin 17 as output, run [echo "out" > /dev/LED_Driver]
+ To set pin 17 as input, run [echo "in" > /dev/LED_Driver]
+ To turn on LED, run [echo "on" > /dev/LED_Driver]
+ To turn off LED, run [echo "off" > /dev/LED_Driver]
+ To show all messages, run [dmesg]
+ To exit driver, run [rmmod LED_Driver]