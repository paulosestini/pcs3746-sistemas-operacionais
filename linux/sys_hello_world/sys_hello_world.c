#include<linux/linkage.h>
#include<linux/module.h>

asmlinkage long sys_hello_world(void)
{
	return pr_info("Hello World From The Kernel!\n");
}
