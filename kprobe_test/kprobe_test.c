#include <linux/kprobes.h>
#include <linux/module.h>
struct kprobe kp = {};

MODULE_LICENSE("GPL");

struct Pair {  
  const char* msg;  
  int code;  
};  
#define  _ARM_REG_DEF(x,y)  { #x, y },  
static struct Pair _arm_register_strings[] = {  
_ARM_REG_DEF(ARM_cpsr   ,16)  
_ARM_REG_DEF(ARM_pc     ,15)  
_ARM_REG_DEF(ARM_lr     ,14)  
_ARM_REG_DEF(ARM_sp     ,13)  
_ARM_REG_DEF(ARM_ip     ,12)  
_ARM_REG_DEF(ARM_fp     ,11)  
_ARM_REG_DEF(ARM_r10        ,10)  
_ARM_REG_DEF(ARM_r9     ,9)  
_ARM_REG_DEF(ARM_r8     ,8)  
_ARM_REG_DEF(ARM_r7     ,7)  
_ARM_REG_DEF(ARM_r6     ,6)  
_ARM_REG_DEF(ARM_r5     ,5)  
_ARM_REG_DEF(ARM_r4     ,4)  
_ARM_REG_DEF(ARM_r3     ,3)  
_ARM_REG_DEF(ARM_r2     ,2)  
_ARM_REG_DEF(ARM_r1     ,1)  
_ARM_REG_DEF(ARM_r0         ,0)  
_ARM_REG_DEF(ARM_ORIG_r0    ,17)  
};  


static struct pt_regs regs;  
static char * name = "load_module";  

static int dump_arm_regs(unsigned long * buf)  
{     
   int i=0;  
   int j=0;  
   char * detail = NULL;  
   for(i=0;i<18;i++)  
   {  
      for (j=0;j<sizeof(_arm_register_strings);j++)  
      {  
         if(_arm_register_strings[j].code==i)  
         {  
            detail = _arm_register_strings[j].msg;  
            break;  
         }  
      }  
      printk(" %s  %02d : 0x%08x  : %10d : %20s\n",name,i,*(buf+i),*(buf+i),detail);  
   }  
   return 0;  
}  

static int dump_arm_stack(unsigned int * _addr , unsigned int addrSize)  
{     
   int i=0;  
   int j=0;  
   int word_per_line=4;  
   unsigned int * addr = _addr;  
   addr = addr + word_per_line*addrSize;  
  
   for(i=0;i<addrSize*2;i++)  
   {  
         char * middlestack = "---";  
         if(i==addrSize)  
         {  
            middlestack = "$$$";  
         }  
         printk(" addr:0x%08x %s 0x%08x 0x%08x 0x%08x 0x%08x \n", \  
                                    (addr-(i*word_per_line+0)),\  
                                    middlestack, \  
                                    *(addr-(i*word_per_line+0)),\  
                                    *(addr-(i*word_per_line+1)),\  
                                    *(addr-(i*word_per_line+2)),\  
                                    *(addr-(i*word_per_line+3)) \  
                                    );  
   }  
   return 0;  
}  

static int dump_arm_parameter(struct pt_regs *regs)  
{     
   int i=0;  
   unsigned int * sp = regs->uregs[13];  
   printk(" func paras maybe : (0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x) \n",\  
                                    regs->uregs[0],\  
                                    regs->uregs[1],\  
                                    regs->uregs[2],\  
                                    regs->uregs[3],\  
                                    *sp,\  
                                    *(sp+1)\  
                                    );  
  
   return 0;  
}  


static void handler_post(struct kprobe *k, struct pt_regs *regs, unsigned long flags)
{
	printk(KERN_INFO "post_handler: addr=0x%p, eflags=0x%lx\n",	k->addr, k->flags);
	return;
}

static int handler_pre(struct kprobe *p, struct pt_regs *regs)  
{  
    printk(" kprobes name is %s pt_regs size is %d \n",p->symbol_name,sizeof(regs->uregs));  
    dump_arm_regs(regs->uregs);  
    dump_arm_stack((unsigned int *)regs->uregs[15],3);  
    dump_arm_parameter(regs);
    return 0;
} 

static int __init init_kprobe_test(void)
{
    int ret;
	kp.pre_handler  = handler_pre;
	kp.post_handler = handler_post;
	kp.symbol_name  = name;
    
	if(ret = register_kprobe(&kp))
	{
		printk(KERN_INFO "Cannot register kprobe.ret=%d\n",ret);
		return 1;
	}
	
	printk(KERN_INFO "kprobe module was loaded.\n");
	return 0;
}

static void __exit exit_kprobe_test(void)
{
	unregister_kprobe(&kp);
	printk(KERN_INFO "kprobe module was unloaded.\n");
}

module_init(init_kprobe_test);
module_exit(exit_kprobe_test);
