#include<linux/sched.h>
#include<linux/list.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LIST ALL PID");

static int pid = 1;
module_param(pid,int,0644);

static int list_pid_kinfolk_int(void)
{
	struct task_struct *p;
	struct list_head *pp;
	struct task_struct * task;
	
	p = pid_task(find_get_pid(pid),PIDTYPE_PID);
	printk(KERN_ALERT"PID: %d    Parent: %s\n",p->pid,p->parent->comm);

    // list all children
	list_for_each(pp,&p->children)
	{
		task = list_entry(pp,struct task_struct, sibling);
		printk(KERN_ALERT"PID: %d    Children: %s\n",task->pid,task->comm);
	}

    // list all sibling
	list_for_each(pp,&p->sibling)
	{
		task = list_entry(pp,struct task_struct, sibling);
		printk(KERN_ALERT"PID: %d    Sibling: %s\n",task->pid,task->comm);
	}
	
	return 0;
}

static void list_pid_kinfolk_exit(void)
{
	printk(KERN_ALERT"I'M LEAVING~~\n");
}

module_init(list_pid_kinfolk_int);
module_exit(list_pid_kinfolk_exit);
