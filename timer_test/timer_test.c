#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/preempt.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#define DEV_NAME    "test-dev"

struct timer_list timer;
void func(unsigned long data)       //tasklet (中断下半部)软中断; 
{
    printk("timer out data: %lu\n", data);
    mod_timer(&timer, jiffies + 3 * HZ);
    //timer.expires = jiffies + 3 * HZ;
    add_timer(&timer);
}

int test_init(void)
{
    printk("test init\n");
    setup_timer(&timer, func, 100);     
    timer.expires = jiffies + 3 * HZ;
    add_timer(&timer);                  //3s 后来定时器中断;
    return 0;
}

void test_exit(void)    //模块拔出时的函数;
{
    printk("test exit\n");
    del_timer(&timer);
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION("V1.0");
MODULE_AUTHOR("xuwei www.uplooking.com");



