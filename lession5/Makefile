obj-m += User_Driver.o
User_Driver-objs := header.o main.o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
