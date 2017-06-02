#include <linux/module.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");

int my_function(void * argc)
{
    printk("<0>in the kernel thread function!\n");
    printk("<0>the current pid is:%d\n",current->pid);
    printk("<0>out the kernel thread function\n");
    return 0;
}

static int __init add_wait_queue_init(void)
{
    struct task_struct *result,*result1;
    int wait_queue_num=0;
    wait_queue_head_t head;
    wait_queue_t data,data1,*curr, *next;

    printk("<0>into add_wait_queue_init.\n");

    result=kthread_run(my_function,NULL,"my_function");
    result1=kthread_run(my_function,NULL,"my_function1");
    
#if 0
    init_waitqueue_head(&head);
    init_waitqueue_entry(&data,result);
    init_waitqueue_entry(&data1,result1);
    add_wait_queue(&head,&data);
    add_wait_queue(&head,&data1);

    list_for_each_entry_safe(curr, next, &(head.task_list), task_list) {
        wait_queue_num++;
        printk("<0>the flag value of the current data of the waitqueue is:%d\n",curr->flags);
        printk("<0>the pid value of the current data of the waitqueue is:%d\n",((struct task_struct *)(curr->private))->pid);
    }
    printk("<0>the value of the wait_queue_num is :%d\n",wait_queue_num);
    printk("<0>the result of the kernel_thread is :%d\n",result->pid);
    printk("<0>the result1 of the kernel_thread is :%d\n",result1->pid);
    printk("<0>the current pid is:%d\n",current->pid);
    printk("<0>out add_wait_queue_init.\n");
#endif
    return 0;
}

static void __exit add_wait_queue_exit(void)
{
    printk("<0>Goodbye add_wait_queue\n");
}

module_init(add_wait_queue_init);
module_exit(add_wait_queue_exit);
