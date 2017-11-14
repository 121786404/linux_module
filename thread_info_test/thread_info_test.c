#include <linux/init.h>
#include <linux/module.h>
#include <asm/thread_info.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BUNFLY");

int thread_info_init(void)
{
	int i = 0;
	struct thread_info *p = NULL;
	p = (struct thread_info *)((unsigned long)&i & ~0x1fff); // 8K aligh
	struct task_struct *t = p->task;
	printk("task name is %s\n", t->comm);

    struct task_struct *head = p->task;
    struct task_struct *tmp = head;
    do{
         printk("task name is %s\n", tmp->comm);
         tmp = container_of(tmp->tasks.next, struct task_struct, tasks);
     }while(tmp != head);
    
	return 0;
}

void thread_info_exit(void)
{
	printk("bye bye\n");
}

module_init(thread_info_init);
module_exit(thread_info_exit);