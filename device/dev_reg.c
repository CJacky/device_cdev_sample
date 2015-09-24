#include <linux/init.h>         /* modules */
#include <linux/module.h>       /* module */
#include <linux/types.h>         /* dev_t type */
#include <linux/kernel.h>
#include <linux/fs.h>           /* chrdev allocation */
#include <linux/cdev.h>
#include <linux/sched.h>
#include <asm/current.h>                   /* Proccess */
#include <asm/uaccess.h>

#include "dev_reg.h"

#define DRIVER_NAME "dev_reg"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Emil Chen");
MODULE_DESCRIPTION("Register Character Device");

dev_t dev;
struct cdev dev_reg_cdev;
static unsigned int dev_reg_major = 0;
module_param(dev_reg_major, uint, 0);

static int dev_reg_open(struct inode *inode, struct file *file)
{       /* open handler */
	printk("%s: major %d minor %d (pid %d)\n", __func__,
			imajor(inode),
			iminor(inode),
			current->pid
		  );

	inode->i_private = inode;
	file->private_data = file;

	printk(" i_private=%p private_data=%p\n",
			inode->i_private, file->private_data);

	return 0;     /* success */
}

static int dev_reg_close(struct inode *inode, struct file *file)
{       /* close handler */
	printk("%s: major %d minor %d (pid %d)\n", __func__,
			imajor(inode),
			iminor(inode),
			current->pid );

	printk(" i_private=%p private_data=%p\n",
			inode->i_private, file->private_data);

	return 0;     /* success */
}

struct file_operations dev_reg_fops = {
	.open = dev_reg_open,
	.release = dev_reg_close,
};

static int __init_dev(void)
{        
	int alloc_ret = 0, cdev_err = 0;

	// alloc_chrdev_region return 0 on success
	alloc_ret = alloc_chrdev_region(&dev, DEVNUM_MINOR_START, DEVNUM_COUNT, DEVNUM_NAME);

	if(alloc_ret){
		printk(KERN_WARNING "%s : could not allocate device\n", __func__);
		goto error;
	}
	else{
		printk(KERN_WARNING "%s : registered with major number:%i, minor number:%i\n",
				__func__, MAJOR(dev), MINOR(dev));
		dev_reg_major = MAJOR(dev);
	}

	cdev_init(&dev_reg_cdev, &dev_reg_fops);
	dev_reg_cdev.owner = THIS_MODULE;

	cdev_err = cdev_add(&dev_reg_cdev, MKDEV(dev_reg_major, 0), DEVNUM_COUNT);
	if(cdev_err){
		printk(KERN_WARNING "%s : could not add cdev\n", __func__);
		goto error;
	}

	printk(KERN_ALERT "%s driver(major %d) installed.\n", DRIVER_NAME, dev_reg_major);

	return 0;

error:
	if (cdev_err == 0)
		cdev_del(&dev_reg_cdev);

	if (alloc_ret == 0)
		unregister_chrdev_region(dev, DEVNUM_COUNT);

	return -1;
}

static void __exit_dev(void)
{
	printk(KERN_INFO "dev_num Module removed.\n");

	/* Free the devices */
	cdev_del(&dev_reg_cdev);
	unregister_chrdev_region(dev,DEVNUM_COUNT);
}

module_init(__init_dev);
module_exit(__exit_dev);
