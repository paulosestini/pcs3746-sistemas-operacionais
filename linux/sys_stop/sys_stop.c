#include <asm/barrier.h>
#include <linux/linkage.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/wait.h>

#define set_task_state(tsk, state_value)		\
	do { (tsk)->state = (state_value); smp_mb(); } while (0)

DECLARE_WAIT_QUEUE_HEAD(queue);

static int autoremove_autofree_wake_function(wait_queue_t *wait, unsigned mode, int sync, void *key)
{
	int ret = default_wake_function(wait, mode, sync, key);

	if (ret) {
		list_del_init(&wait->task_list);
		kfree(wait);
	}
	return ret;
}

static long stop_process(struct task_struct *p)
{
	unsigned long flags;
	wait_queue_t *wait = kmalloc(sizeof(*wait), GFP_KERNEL);
	if (!wait)
		return -1;
	init_wait(wait);
	wait->private = p;
	wait->func = autoremove_autofree_wake_function;
	wait->flags |= WQ_FLAG_EXCLUSIVE;

	spin_lock_irqsave(&queue.lock, flags);
	__add_wait_queue_tail(&queue, wait);
	set_task_state(p, TASK_STOPPED);
	spin_unlock_irqrestore(&queue.lock, flags);

	return 0;
}

asmlinkage long sys_stop_process(int pid)
{
	struct task_struct *p = find_task_by_vpid(pid);
	return p ? stop_process(p) : -1;
}

asmlinkage long sys_continue_process(void)
{
	__wake_up(&queue, TASK_STOPPED, 1, NULL);
	return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tiago Koji Castro Shibata <tishi@linux.com>");
MODULE_AUTHOR("Eric Rodrigues Pires <ericpires9@gmail.com>");
MODULE_DESCRIPTION("Process stop/continue module");
