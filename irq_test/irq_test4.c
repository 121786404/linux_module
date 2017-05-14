#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

#include <asm/irq.h>

#include <mach/irqs.h>
#include <mach/map.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-samsung.h>

#include <plat/cpu.h>
#include <plat/gpio-core.h>
#include <plat/gpio-cfg.h>
#include <plat/gpio-cfg-helpers.h>
#include <plat/pm.h>

//中断处理函数处于中断上下文，中断上下文中不允许睡眠
irqreturn_t do_eint0(int no, void *data)
{
	printk("hello eint0 %d\n", no);
	printk("contend:%lu\n", in_interrupt());
	//ssleep(5);

	return IRQ_HANDLED;//IRQ_NONE;	
}

//inmod xxx.ko
//处于进程上下文
static __init int irq_test_init(void)
{
	int ret;
	
	printk("contend:%lu\n", in_interrupt());
	printk("current:%s\n", current->comm);
	
	ret = request_irq(IRQ_EINT(0), do_eint0, IRQF_TRIGGER_FALLING, "eint0", NULL);
	if(ret < 0)
		return ret;

	return 0;
}

static __exit void irq_test_exit(void)
{
	free_irq(IRQ_EINT(0), NULL);
}

module_init(irq_test_init);
module_exit(irq_test_exit);

MODULE_LICENSE("GPL");
