#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static DECLARE_WAIT_QUEUE_HEAD(wq);
static LIST_HEAD(data_queue);

struct device_data {
	struct list_head head;
	char data;
};

static int get_data(char *c)
{
	struct device_data *entry;

	if (list_empty(&data_queue))
		return -1;

	entry = list_first_entry(&data_queue, struct device_data, head);
	*c = entry->data;
	list_del(&entry->head);
	kfree(entry);
	return 0;
}

static long put_data(char c)
{
	struct device_data *entry = kmalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry) {
		pr_debug("Device queue data allocation failed\n");
		return -1;
	}
	entry->data = c;
	list_add_tail(&entry->head, &data_queue);
	return 0;
}

static ssize_t blocking_dev_read(struct file *filp, char __user *buffer,
	size_t length, loff_t *ppos)
{
	char c;
	if (!length)
		return 0;
	if (wait_event_interruptible(wq, !get_data(&c)))
		return -ERESTARTSYS;

	// simple_read_from_buffer will update the file offset and check whether
	// it fits the available data argument (1), use copy_to_user instead.
	return 1 - copy_to_user(buffer, &c, 1);
}

asmlinkage long sys_write_device(int data)
{
	int rc = put_data(data);
	if (rc)
		return rc;
	wake_up_interruptible(&wq);
	return 0;
}

static const struct file_operations blocking_dev_fops = {
	.owner = THIS_MODULE,
	.read  = blocking_dev_read
};

static struct miscdevice id_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "blocking_dev",
	.fops = &blocking_dev_fops
};

static int __init blocking_dev_init(void)
{
	int retval;

	retval = misc_register(&id_misc_device);
	if (retval)
		pr_err("blocking_dev_dev: misc_register %d\n", retval);
	return retval;
}

static void __exit blocking_dev_exit(void)
{
	misc_deregister(&id_misc_device);
}

module_init(blocking_dev_init);
module_exit(blocking_dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tiago Koji Castro Shibata <tishi@linux.com>");
MODULE_DESCRIPTION("Blocking device sample");
