#include <linux/linkage.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>

static LIST_HEAD(stack);

struct stack_value {
	int value;
	struct list_head stack;
};

static unsigned long count;
static struct kobject *sys_stack_kobject;

asmlinkage long sys_stack_push(int value)
{
	struct stack_value *new_stack_value;

	new_stack_value = kmalloc(sizeof(*new_stack_value), GFP_KERNEL);
	new_stack_value->value = value;

	list_add(&new_stack_value->stack, &stack);
	count++;

	return 0;
}

asmlinkage long sys_stack_pop(void)
{
	int value;
	struct stack_value *top_stack_value;

	if (list_empty(&stack)) {
		pr_debug("EMPTY STACK\n");
		return -1;
	}

	top_stack_value = list_first_entry(&stack, struct stack_value, stack);
	value = top_stack_value->value;
	list_del(&top_stack_value->stack);
	kfree(top_stack_value);
	count--;

	return value;
}

static ssize_t count_show(struct kobject *kobj, struct kobj_attribute *attr,
			    char *buf)
{
	return sprintf(buf, "%lu\n", count);
}

static struct kobj_attribute count_attribute = __ATTR_RO(count);

static struct attribute *attrs[] = {
	&count_attribute.attr,
	NULL,
};

static int __init sys_stack_init(void)
{
	int retval;

	sys_stack_kobject = kobject_create_and_add("sys_stack", kernel_kobj);
	if (!sys_stack_kobject)
		pr_debug("stack_dev: can't create kobject");

	retval = sysfs_create_file(sys_stack_kobject, *attrs);
	if (retval) {
		pr_debug("stack_dev: can't create sysfs group");
		kobject_put(sys_stack_kobject);
		return retval;
	}

	return 0;
}

static void __exit sys_stack_exit(void)
{
	kobject_put(sys_stack_kobject);
}

module_init(sys_stack_init);
module_exit(sys_stack_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tiago Koji Castro Shibata <tishi@linux.com>");
MODULE_AUTHOR("Eric Rodrigues Pires <ericpires9@gmail.com>");
MODULE_DESCRIPTION("Simple stack module");
