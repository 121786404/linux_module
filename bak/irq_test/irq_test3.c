#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

irqreturn_t do_eint0(int no, void *data)
{
	//中断上下文
	printk("hello eint0 %d\n", no);

	return IRQ_HANDLED;//IRQ_NONE;	
}

void fun(void)
{
	unsigned long flags;
	//local_irq_disable();	
	local_irq_save(flags);
	printk("fun\n");
	local_irq_restore(flags);
	//local_irq_enable();
}

static __init int irq_test_init(void)
{
	int ret;
	unsigned long flags;

	ret = request_irq(IRQ_EINT(0), do_eint0, IRQF_TRIGGER_FALLING, "eint0", NULL);
	if(ret < 0)
		return ret;

//	local_irq_disable();
	local_irq_save(flags);
	fun();
	mdelay(5000);
	local_irq_restore(flags);
//	local_irq_enable();

	return 0;
}

static __exit void irq_test_exit(void)
{
	free_irq(IRQ_EINT(0), NULL);
}

module_init(irq_test_init);
module_exit(irq_test_exit);

MODULE_LICENSE("GPL");
