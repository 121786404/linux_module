#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

irqreturn_t do_eint1(int no, void *data)
{
	printk("hello eint1 %d\n", no);

	//while(1);

	return IRQ_HANDLED;//IRQ_NONE;	
}

irqreturn_t do_eint0(int no, void *data)
{
	printk("hello eint0 %d\n", no);

	return IRQ_HANDLED;//IRQ_NONE;	
}

static __init int irq_test_init(void)
{
	int ret;

	ret = request_irq(IRQ_EINT(0), do_eint0, IRQF_TRIGGER_FALLING | IRQF_SHARED, "eint0", do_eint0);
	if(ret < 0)
		return ret;
	ret = request_irq(IRQ_EINT(0), do_eint1, IRQF_TRIGGER_FALLING | IRQF_SHARED, "eint1", do_eint1);
	if(ret < 0)
		goto request_eint1_error;

	return 0;
request_eint1_error:
	free_irq(IRQ_EINT(0), do_eint0);
	return ret;
}

static __exit void irq_test_exit(void)
{
	free_irq(IRQ_EINT(0), do_eint1);
	free_irq(IRQ_EINT(0), do_eint0);
}

module_init(irq_test_init);
module_exit(irq_test_exit);

MODULE_LICENSE("GPL");
