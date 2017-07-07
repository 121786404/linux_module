#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#define SOFT_FLOAT_TEST

static int  __init init_float(void)
{
#ifdef SOFT_FLOAT_TEST
	double a = 3.1415926; 
	double b = 1.2345678; 
	printk(KERN_INFO "[soft-float] hex(a) = %llx\n", *((u64*)&a)); 
	printk(KERN_INFO "[soft-float] hex(b) = %llx\n", *((u64*)&b)); 
	b = a * b; 
	printk(KERN_INFO "[soft-float] hex(a*b) = %llx\n", *((u64*)&b)); 
#else
	double a, b;
	kernel_fpu_begin(); 
	a = 3.1415926; 
	b = 1.2345678; 
	printk(KERN_INFO "[hard-float] hex(a) = %llx\n", *((u64*)&a)); 
	printk(KERN_INFO "[hard-float] hex(b) = %llx\n", *((u64*)&b)); 
	b = a * b; 
	printk(KERN_INFO "[hard-float] hex(a*b) = %llx\n", *((u64*)&b)); 
	kernel_fpu_end(); 
#endif	
	return 0; 
}

static void __exit exit_float(void)
{
#ifdef SOFT_FLOAT_TEST
	printk(KERN_INFO "[soft-float] Goodbye.\n");
#else
	printk(KERN_INFO "[hard-float] Goodbye.\n");
#endif	
}

module_init(init_float); 
module_exit(exit_float); 


