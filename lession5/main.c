/*
* @file      TramAnh.c
* @author    Bui Thien An
* @date 2    Nov 2018
* @version   1.0
* @brief     Example about simple character driver
*/
#include "header.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define DEVICE_NAME "User_Driver"
#define CLASS_NAME  "User_Driver"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bui Thien An");
MODULE_DESCRIPTION("A simple Linux character driver");
MODULE_VERSION("0.1");

static int major_number;
static struct class* char_class;
static struct device* char_device;

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
	printk(KERN_INFO "Processer %s:: Included haha %d \n",__func__,add(major_number,major_number));
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
	return 0;
}



static ssize_t dev_write(struct file *filep, char const *buffer, size_t len, loff_t *offset)
{
	
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	return 0;
}

module_init(Driver_init);
module_exit(Driver_exit);
