#include<linux/sched.h>
#include<linux/list.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LIST ALL PID");

static int list_process_init(void)
{
	struct task_struct *p;
	for_each_process(p)
	{
		if(p->mm==NULL)
			printk(KERN_ALERT"Process Name: %s   PID: %d   Statue:%ld\n",p->comm,p->pid,p->state);
	}
	return 0;
}

static void list_process_exit(void)
{
}

module_init(list_process_init);
module_exit(list_process_exit);
