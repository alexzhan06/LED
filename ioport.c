#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <linux/slab.h>
#define S5PV210_GPJ2CON 	(S5PV210_GPJ2_BASE + 0x00)
#define S5PV210_GPJ2DAT		(S5PV210_GPJ2_BASE + 0x04)
#define S5PV210_GPJ2PUD	 	(S5PV210_GPJ2_BASE + 0x08)
#define LED_MAJOR		0
static int LED_major = LED_MAJOR;
struct resource *IO_port_resource;  //Save allocated IO port resource
static struct class *ioport_class;
struct LED_dev
{
	struct cdev cdev;
};
struct LED_dev  *LED_devp;
/****************************************************************
 *  name: LED_open()
 *  function: open device file in correspond to open syscal in user space
 *  input parameters: node for device file
 *  output parameters: none
 * **************************************************************/
 int LED_open(struct inode *inode, struct file *filp)
{
//   struct LED_dev  *dev;
//   dev = container_of(inode->i_cdev, struct LED_dev , cdev);
//   filp->private_data = dev;
    printk("In the open process! turn off the LED!\n");
    outl(0x1111 | inl((unsigned long) S5PV210_GPJ2CON), (unsigned long) S5PV210_GPJ2CON);
    outl(0xFFAA | inl((unsigned long) S5PV210_GPJ2PUD), (unsigned long) S5PV210_GPJ2PUD);
    outl(0xF0 | inl((unsigned long) S5PV210_GPJ2DAT), (unsigned long) S5PV210_GPJ2DAT);
   return 0;
}
 /*********************************************************
  * Another comment
  *********************************************************/
int LED_release(struct inode *inode, struct file *filp)
{
	 return 0;
}
static ssize_t LED_read(struct file *filp, char __user  *buf, size_t size, loff_t *ppos)
{
   int ret = 0;
   return ret;
   }
static ssize_t LED_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
   int ret = 0;
   int retval = 0;
   unsigned char userbuf;

    retval = copy_from_user(&userbuf, buf, sizeof(userbuf));
    if (retval)
   {
	    ret = -EFAULT;
   }  else
   {
	   outl(userbuf, (unsigned long) S5PV210_GPJ2DAT);
	    printk(" write data from user to ioport!\n");
   }
     return ret;
 }

static const struct file_operations LED_fops=
{
		.owner = THIS_MODULE,
		.read = LED_read,
		.write = LED_write,
		.open = LED_open,
		.release = LED_release,
};

static void LED_setup_cdev(struct LED_dev *dev, int index)
{
	 int err, devno=MKDEV(LED_major, index);
	 cdev_init(&dev->cdev, &LED_fops);
	 dev->cdev.owner = THIS_MODULE;
	 dev->cdev.ops = &LED_fops;
	 err = cdev_add(&dev->cdev, devno, 1);

	 if (err)
		 printk(KERN_NOTICE  "Error %d adding LED%d", err, index);
	 ioport_class = class_create(THIS_MODULE, "ioport");
	 device_create(ioport_class, NULL, devno, NULL, "ioport");
}
int LED_init(void)
{
	 int result;
	dev_t devno = MKDEV(LED_major, 0);
	 if (LED_major)
		 result = register_chrdev_region(devno, 1, "ioport");
	 else
	 {
		 result = alloc_chrdev_region(&devno, 0, 1, "ioport");
		 LED_major = MAJOR(devno);
	 }
	 if (result<0)
		 return result;
	 LED_devp = kmalloc(sizeof(struct LED_dev), GFP_KERNEL);
	 if (!LED_devp)
	 {
		 result = -ENOMEM;
		 goto fail;
	 }
		 memset(LED_devp, 0, sizeof(struct LED_dev));
		 LED_setup_cdev(LED_devp, 0);
		 if ((IO_port_resource=request_region((unsigned long) S5PV210_GPJ2CON, 0x0c, "ioport"))==NULL)
			 goto fail;
		 else
			 return 0;

	fail: unregister_chrdev_region(devno,1);
	return result;
}

void LED_exit(void)
{
	if (IO_port_resource!=NULL)
		release_region((unsigned long) S5PV210_GPJ2CON, 0x0c);
	cdev_del(&LED_devp->cdev);
	kfree(LED_devp);
	unregister_chrdev_region(MKDEV(LED_major,0),1);
	device_destroy(ioport_class, MKDEV(LED_major,0));
	class_destroy(ioport_class);
}

MODULE_AUTHOR("AK-47");
MODULE_LICENSE("Dual BSD/GPL");
module_init(LED_init);
module_exit(LED_exit);


