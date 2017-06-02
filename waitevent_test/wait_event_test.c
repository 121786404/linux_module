#include <linux/module.h>

#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");

static wait_queue_head_t wq;
static int condition;

static int test_thread(void *arg)
{
        printk(KERN_INFO "thread %d wait for condition\n", (int)current->pid);
        wait_event(wq, condition);
        printk(KERN_INFO "thread %d exit because condition appear\n", (int)current->pid);
        return 0;
}

int __init hello_waitqueue_init(void)
{
        struct task_struct *task;
        init_waitqueue_head(&wq);
        task = kthread_create(test_thread, NULL, "test_thread");
        if (!task) {
                printk(KERN_ERR "failed to create thread\n");
                return -1;
        }
        wake_up_process(task);
        return 0;
}

void __exit hello_waitqueue_exit(void)
{
        printk(KERN_INFO "wait up\n");
        condition = 1;
        wake_up(&wq);
}

module_init(hello_waitqueue_init);
module_exit(hello_waitqueue_exit);
