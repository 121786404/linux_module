/*
 * http://blog.csdn.net/qb_2008/article/details/6837262
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

static struct task_struct *period_tsks;
static struct completion wakeups;

static int period_thread(void *data)
{
    printk(KERN_INFO "%s %d\n" ,__func__, __LINE__);
    do {
            printk(KERN_INFO "%s %d\n" ,__func__, __LINE__);
            wait_for_completion(&wakeups);
    printk(KERN_INFO "%s %d\n" ,__func__, __LINE__);
    } while (!kthread_should_stop());
    printk(KERN_INFO "%s %d\n" ,__func__, __LINE__);
    return ;
}

static int hello_init(void)
{
    printk(KERN_INFO "%s %d\n" ,__func__, __LINE__);
    init_completion(&wakeups);
    period_tsks = kthread_run(period_thread, NULL, "period_thread");
    if (IS_ERR(period_tsks))
            printk(KERN_INFO "create period_thread failed!\n");
    printk(KERN_INFO "%s %d\n" ,__func__, __LINE__);
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_INFO "%s %d\n" ,__func__, __LINE__);

    complete(&wakeups);
    if (!IS_ERR(period_tsks))
       kthread_stop(period_tsks);

    printk(KERN_INFO "%s %d\n" ,__func__, __LINE__);
}

module_init(hello_init);
module_exit(hello_exit);
