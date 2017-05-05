#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/kthread.h>  
#include <linux/completion.h>  
#include <linux/delay.h> 

MODULE_LICENSE("Dual BSD/GPL");  
  
#define PERIOD_THREAD_NUM 5  
  
static int periods[PERIOD_THREAD_NUM] =   
    { 1, 2, 4, 8, 16 };  
  
static struct task_struct *period_tsks[PERIOD_THREAD_NUM];  
  
static struct task_struct* watchdog_tsk;  
  
static struct completion wakeups[PERIOD_THREAD_NUM];  
  
static int exit_flag = 0;  
  
static int period_thread(void *data)  
{  
    int k = (int)data;  
    int count = -1;  
  
    do{  
        printk("thread%d: period=%ds, count=%d\n", k, periods[k], ++count);  
        wait_for_completion(&wakeups[k]);  
    }while(!exit_flag);  
    return count;  
}  
  
static int watchdog_thread(void *data)  
{  
    int k;  
    int count = 0;  
      
    do{  
        msleep(1000);  
        count++;  
        for(k=0; k<PERIOD_THREAD_NUM; k++){  
            if (count%periods[k] == 0)  
                complete(&wakeups[k]);  
        }  
    }while(!exit_flag);  
    return count;  
}  
  
static int hello_init(void)  
{  
    int k;  
  
    printk(KERN_INFO "Hello, world!\n");  
  
    for(k=0; k<PERIOD_THREAD_NUM; k++){  
        init_completion(&wakeups[k]);  
    }  
  
    watchdog_tsk = kthread_run(watchdog_thread, NULL, "watchdog_thread");  
  
    if(IS_ERR(watchdog_tsk)){  
        printk(KERN_INFO "create watchdog_thread failed!\n");  
        return 1;  
    }  
  
    for(k=0; k<PERIOD_THREAD_NUM; k++){  
        period_tsks[k] = kthread_run(period_thread, (void*)k, "period_thread%d", k);  
        if(IS_ERR(period_tsks[k]))  
            printk(KERN_INFO "create period_thread%d failed!\n", k);  
    }  
    return 0;  
}  
  
static void hello_exit(void)  
{  
    int k;  
    int count[5], watchdog_count;  
  
    printk(KERN_INFO "Hello, exit!\n");  
    exit_flag = 1;  
    for(k=0; k<PERIOD_THREAD_NUM; k++)  
        complete_all(&wakeups[k]);  
  
    for(k=0; k<PERIOD_THREAD_NUM; k++){  
        count[k] = 0;  
        if(!IS_ERR(period_tsks[k]))  
            count[k] = kthread_stop(period_tsks[k]);  
    }  
    watchdog_count = 0;  
    if(!IS_ERR(watchdog_tsk))  
        watchdog_count = kthread_stop(watchdog_tsk);  
  
    printk("running total time: %ds\n", watchdog_count);  
    for(k=0; k<PERIOD_THREAD_NUM; k++)  
        printk("thread%d: period %d, running %d times\n", k, periods[k], count[k]);  
}  
  
module_init(hello_init);  
module_exit(hello_exit);