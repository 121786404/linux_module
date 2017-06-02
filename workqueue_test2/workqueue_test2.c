#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

static struct work_struct hello_work_struct;

void hello_work_func(struct work_struct *work)
{
        printk(KERN_INFO "hello workqueue\n");
}

int hello_workqueue_init(void)
{
        INIT_WORK(&hello_work_struct, hello_work_func);
        queue_work(system_wq, &hello_work_struct);

        return 0;
}

void hello_workqueue_exit(void)
{
}

module_init(hello_workqueue_init);
module_exit(hello_workqueue_exit);
MODULE_LICENSE("GPL");
