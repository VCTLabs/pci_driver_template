/*
  chardev.c
    - kernel module character devices implementation
 */

#include <linux/cdev.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,9,16)
#include <linux/device/class.h>
#else
#include <linux/device.h>
#endif
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include "chardev.h"
#include "pcie385asfp_user.h"

#define MAX_DEV 4
#define DEV_MAJOR 89

static int sfpdev_open(struct inode *inode, struct file *file);
static int sfpdev_release(struct inode *inode, struct file *file);
static long sfpdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t sfpdev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t sfpdev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

static const struct file_operations sfpdev_fops = {
	.owner      = THIS_MODULE,
	.open       = sfpdev_open,
	.release    = sfpdev_release,
	.unlocked_ioctl = sfpdev_ioctl,
	.read       = sfpdev_read,
	.write       = sfpdev_write
};

struct sfp_device_data {
	struct device* sfpdev;
	struct cdev cdev;
};

struct sfp_device_private {
	uint8_t chnum;
	struct pcie385asfp_driver* drv;
};

static int dev_major = 0;
static struct class *sfpclass = NULL;
static struct sfp_device_data sfpdev_data[MAX_DEV];
static struct pcie385asfp_driver* drv_access = NULL;

static int pcie385asfp_uevent(const struct device *dev, struct kobj_uevent_env *env)
{
	add_uevent_var(env, "DEVMODE=%#o", 0666);

	return 0;
}

int create_char_devs(struct pcie385asfp_driver* drv)
{
	int err, i;
	dev_t dev;

	err = alloc_chrdev_region(&dev, 0, MAX_DEV, "pcie385asfp");

	dev_major = MAJOR(dev);

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,9,16)
	sfpclass = class_create("pcie385asfp-dev");
#else
	sfpclass = class_create(THIS_MODULE, "pcie385asfp-dev");
#endif

	sfpclass->dev_uevent = pcie385asfp_uevent;

	for (i = 0; i < MAX_DEV; i++) {
		cdev_init(&sfpdev_data[i].cdev, &sfpdev_fops);
		sfpdev_data[i].cdev.owner = THIS_MODULE;
		cdev_add(&sfpdev_data[i].cdev, MKDEV(dev_major, i), 1);

		sfpdev_data[i].sfpdev = device_create(sfpclass, NULL, MKDEV(dev_major, i), NULL, "pcie385asfp-%d", i);
	}

	drv_access = drv;

	return 0;
}

int destroy_char_devs(void)
{
	int i;

	for (i = 0; i < MAX_DEV; i++) {
		device_destroy(sfpclass, MKDEV(dev_major, i));
	}

	class_unregister(sfpclass);
	class_destroy(sfpclass);
	unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);

	return 0;
}

static int sfpdev_open(struct inode *inode, struct file *file)
{
	struct sfp_device_private* sfp_priv;
	unsigned int minor = iminor(inode);

	sfp_priv = kzalloc(sizeof(struct sfp_device_private), GFP_KERNEL);
	sfp_priv->drv = drv_access;
	sfp_priv->chnum = minor;

	file->private_data = sfp_priv;

	return 0;
}

static int sfpdev_release(struct inode *inode, struct file *file)
{
	struct sfp_device_private* priv = file->private_data;

	kfree(priv);

	priv = NULL;

	return 0;
}

static long sfpdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct sfp_device_private* drv = file->private_data;

	/* big switch (cmd) here toggles register bits */

	return 0;
}

static ssize_t sfpdev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	struct sfp_device_private* drv = file->private_data;
	uint32_t data;

	// data = get_channel_data(drv->drv, drv->chnum);
	data = 5;

	if (copy_to_user(buf, &data, count)) {
		return -EFAULT;
	}

	return count;
}

static ssize_t sfpdev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	return count;
}
