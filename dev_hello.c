//query_ioctl.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
 
#include "dev_hello.h"

/* uncomment the next line to enable debug output to the kernel CLI */
#define DEBUG_PRINTS 1 
#ifdef DEBUG_PRINTS
#define debug_print(fmt, args...) printk("%s#%d: " fmt, __FUNCTION__, __LINE__, ## args)
#else
#define debug_print(...)
#endif

#define DEV_NAME "dev_hello"
#define FIRST_MINOR 0
#define MINOR_CNT 1
 
static int dev_major =   0;
static int dev_minor =   0; 

 
static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

dev_data_arg_t dev_data={ 0x01,0x02,0x03,0x04};      
static char   message[256] = {0}; 
static short  size_of_message;    
 
static int dev_open(struct inode *i, struct file *f)
{
    debug_print("Device has been opened.\n");	
    return 0;
}
static int dev_close(struct inode *i, struct file *f)
{
	debug_print("Device successfully closed.\n");
    return 0;
}

static long dev_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
 
    switch (cmd)
    {
        case DEV_DATA_GET_VARIABLES:
            if (copy_to_user((dev_data_arg_t *)arg, &dev_data, sizeof(dev_data_arg_t)))
            {
                return -EACCES;
            }
            break;
        case DEV_DATA_SET_VARIABLES:
            if (copy_from_user(&dev_data, (dev_data_arg_t *)arg, sizeof(dev_data_arg_t)))
            {
                return -EACCES;
            }

            break;

        default:
            return -EINVAL;
    }
 
    return 0;
}


 
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
} 

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "EBBChar: Received %zu characters from the user\n", len);
   return len;
} 
 
static struct file_operations query_fops =
{
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,	
    .release = dev_close,
    .unlocked_ioctl = dev_ioctl
};
 
static int __init dev_hello_init(void)
{
    int ret;
    struct device *dev_ret;
 	debug_print("Device init:Start.\n");
 	if (dev_major) {
		dev = MKDEV(dev_major, dev_minor);
		ret = register_chrdev_region(dev, MINOR_CNT, DEV_NAME);
	} else {
		ret = alloc_chrdev_region(&dev, dev_minor, MINOR_CNT,DEV_NAME);
		dev_major = MAJOR(dev);
	}
	if (ret < 0) {
		debug_print("%s: can't get major %d\n",DEV_NAME,dev_major);
		return ret;
	}

    cdev_init(&c_dev, &query_fops);
 
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
     
    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "dev_hello")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
 
 	debug_print("Device init:End\n"); 
    return 0;
}
 
static void __exit dev_hello_exit(void)
{
 	debug_print("Device exit:Start\n"); 		
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
 	debug_print("Device exit:End\n"); 	
}
 
module_init(dev_hello_init);
module_exit(dev_hello_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("JC Yu <email:jcsteven.yu@gmail.com>");
MODULE_DESCRIPTION("Sample dev_hello driver for ioctl(),read() and Write Char Driver");
MODULE_VERSION("1.0"); 
