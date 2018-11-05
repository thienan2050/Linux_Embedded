/*
* @file      TramAnh.c
* @author    Bui Thien An
* @date 2    Nov 2018
* @version   1.0
* @brief 	 Example about simple character driver, allow user to read and write information from device file
* @Ref		 For more information about Register, please reference page 90, BCM2835 ARM Peripherals
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define DEVICE_NAME "LED_driver"
#define CLASS_NAME  "LED_driver_character"

#define GPIO_BASE 			0x3F200000	//base address of GPIO, this is a vitural space on Linux, the physical address is 0x7E000000
#define GPIO_PIN			17			//this pin will connect to LED
#define GPIOFSEL1			1			//this register will set direction of pin 17 to input/output
#define GPIO_SET0			7			//this register will set pin 17
#define GPIO_CLR0			10			//this register will clear pin 17
#define GPIO_LVL0			13


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bui Thien An");
MODULE_DESCRIPTION("A simple Linux character driver");
MODULE_VERSION("0.1");

static int major_number;
static char message[256];
static char *tempmess;
static struct class* char_class;
static struct device* char_device;
static unsigned int *ui32Reg;
unsigned int *ui32SetDir,*ui32SetBit,*ui32ClrBit;


static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops=
{
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

static int __init Driver_init(void)
{
	ui32Reg = (unsigned int *)ioremap(GPIO_BASE, 0x100);
	if(ui32Reg == NULL)
	{
		printk(KERN_ALERT "Processer %s:: Can not map to to Virtual Address\n",__func__);
		return -ENOMEM;
	}
	tempmess = message;
	printk(KERN_INFO "Processer %s:: Driver is now initializing \n",__func__);
	/*  Allocate major_number for character device  */
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if(major_number < 0)
	{
		printk(KERN_ALERT "Processer %s:: Can not allocate major number \n",__func__);
		return major_number;
	}
	printk(KERN_INFO "Processer %s:: Allocate sucessfull, major number is: %d",__func__,major_number);
	/*  Register Class for character device  */
	char_class = class_create(THIS_MODULE, CLASS_NAME);
	if(IS_ERR(char_class))
	{
		printk(KERN_ALERT "Processer %s:: Can not register Class for character device \n",__func__);
		unregister_chrdev(major_number, DEVICE_NAME);
		return PTR_ERR(char_class);
	}
	printk(KERN_INFO "Processer %s:: Register sucessfull Class for character device \n",__func__);
	/* Register character device */
	char_device = device_create(char_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
	if (IS_ERR(char_device))
	{
		printk(KERN_ALERT "Processer %s:: Can not create character device \n",__func__);
		class_destroy(char_class);
		unregister_chrdev(major_number, DEVICE_NAME);
		return PTR_ERR(char_device);
	}
	printk(KERN_INFO "Processer %s:: Initialize sucessfull in %s function \n",__func__,__func__);
	return 0;
}

static void __exit Driver_exit(void)
{
	device_destroy(char_class, MKDEV(major_number, 0));
	class_unregister(char_class);
	class_destroy(char_class);
	unregister_chrdev(major_number, DEVICE_NAME);
	printk(KERN_INFO "Processer %s:: Driver is now remove from KERNEL \n",__func__);
	
}

static int dev_open(struct inode *inodep, struct file *filep)
{
	return 0;
}
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int error=0;
	int maxsize;
	maxsize=strlen(tempmess);
	if(maxsize <= 0)
	{
		tempmess = message;
		return 0;
	}
	maxsize = maxsize > len ? len : maxsize;
	printk(KERN_INFO "%s, Message = %s\n",__func__,message);
	error = copy_to_user(buffer, message, maxsize);
	if (error != 0)
	{
		printk(KERN_INFO "%s: Khong the ghi du lieu vao bo nho cua user \n",__func__);
		return -EFAULT;
	}
	printk(KERN_INFO "%s: gui chuoi %s len user-space \n",__func__,message);
	return strlen(message);
}



static ssize_t dev_write(struct file *filep, char const *buffer, size_t len, loff_t *offset)
{
	long int error = 0;
	memset(message, 0, sizeof(message));	//clear data in message
	error = copy_from_user(message,buffer,len - 1);
	if(error != 0)
	{
		printk(KERN_INFO "Processer %s:: Can not receive data from User!!! Error = %ld \n",__func__,error);
	}
	printk(KERN_INFO "Processer %s:: message '%s' is written to  buffer \n",__func__,message);
	if((message[0]=='o') && (message[1]=='u') && (message[2]=='t'))
	{
		ui32SetDir = (unsigned int *)(ui32Reg + GPIOFSEL1);			//pointer is now pointing to 0x3F200000 + 1(unsigned int = 4) = 0x3F200004
		(*ui32SetDir) = ((*ui32SetDir) & (~(7 << 21))) | (1 << 21); //set pin 17 is output: clear bit [23:21], then set bit 21 to 1: [23:21:=001
	}
	if((message[0]=='o') && (message[1]=='n'))
	{
		ui32SetBit = (unsigned int *)(ui32Reg + GPIO_SET0);			//pointer is now pointing to 0x3F200000 + 7(unsigned int = 4) = 0x3F20001C
		(*ui32SetBit) |= 1 << GPIO_PIN;								//set bit 17 to 1
	}
	if((message[0]=='o') && (message[1]=='f') && (message[2]=='f'))
	{
		ui32ClrBit = (unsigned int *)(ui32Reg + GPIO_CLR0);			//pointer is now pointing to 0x3F200000 + 10(unsigned int = 4) = 0x3F200028
		(*ui32ClrBit) |= 1 << GPIO_PIN;								//set bit 17 to 1
	}
	return len;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	return 0;
}

module_init(Driver_init);
module_exit(Driver_exit);
