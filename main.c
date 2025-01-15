#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "charDrv"
#define CLASS_NAME "charDrv"

static int majorNumber;
static struct class *charDrvClass = NULL;
static struct device *charDrvDev = NULL;
static struct cdev charDrvCdev;
static char kernelBuffer[256];
static int idx = 0;

static int open(struct inode *inode, struct file *file)
{
  printk(KERN_ALERT "Device opened\n");
  return 0;
}

static ssize_t read(struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
  printk(KERN_ALERT "Reading from device\n");
  int res = copy_to_user(buffer, kernelBuffer, 256);
  return res;
}

static ssize_t write(struct file *file, const char __user *buffer, size_t len, loff_t *offset)
{
  printk(KERN_ALERT "Writing to device\n");
  if (idx > 255)
  {
    idx = 0;
  }
  kernelBuffer[idx] = buffer;
  idx++;
  return len;
}

static int release(struct inode *inode, struct file *file)
{
  printk(KERN_ALERT "Device closed\n");
  return 0;
}

static const struct file_operations fops = {
    .open = open,
    .read = read,
    .write = write,
    .release = release,
    .owner = THIS_MODULE,
};

static int __init openDriver(void)
{
  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  charDrvClass = class_create(THIS_MODULE, CLASS_NAME);
  charDrvDev = device_create(charDrvClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  cdev_init(&charDrvCdev, &fops);
  printk(KERN_ALERT "DRIVER LOADED\n");
  return 0;
}

static void __exit closeDriver(void)
{
  cdev_del(&charDrvCdev);
  device_destroy(charDrvClass, MKDEV(majorNumber, 0));
  class_destroy(charDrvClass);
  unregister_chrdev(majorNumber, DEVICE_NAME);
  printk(KERN_ALERT "REMOVED\n");
}

MODULE_LICENSE("GPL");
module_init(openDriver);
module_exit(closeDriver);
