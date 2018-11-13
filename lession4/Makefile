obj-m += lcd_driver1.o
lcd_driver1-objs := lcd.o lcd_ctl.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

