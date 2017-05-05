#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

typedef struct {
        int counter;
} __atomic_t;

#define prefetchw(x) __builtin_prefetch(x,1)

static inline void __atomic_add(int i, __atomic_t *v)
{
        unsigned long tmp;
        int result;

        prefetchw(&v->counter);
        __asm__ __volatile__("@ __atomic_add\n"
"1:     ldrex   %0, [%3]\n"
"       add     %0, %0, %4\n"
"       strex   %1, %0, [%3]\n"
"       teq     %1, #0\n"
"       bne     1b"
        : "=&r" (result), "=&r" (tmp), "+Qo" (v->counter)
        : "r" (&v->counter), "Ir" (i)
        : "cc");
}

static inline void __atomic_add_fail(int i, __atomic_t *v)
{
        unsigned long tmp;
        int result;

        prefetchw(&v->counter);
        __asm__ __volatile__("@ __atomic_add\n"
"1:     ldrex   %0, [%3]\n"
"       ldrex   %0, [%3]\n"
"       add     %0, %0, %4\n"
"       strex   %1, %0, [%3]\n"
"       strex   %1, %0, [%3]\n"
"       teq     %1, #0\n"
"       beq     1b" /* the 2nd strex should fail */
        : "=&r" (result), "=&r" (tmp), "+Qo" (v->counter)
        : "r" (&v->counter), "Ir" (i)
        : "cc");
}

static inline void __atomic_add_hang(int i, __atomic_t *v)
{
        unsigned long tmp;
        int result;

        prefetchw(&v->counter);
        __asm__ __volatile__("@ __atomic_add\n"
"1:     ldrex   %0, [%3]\n"
"       ldrex   %0, [%3]\n"
"       add     %0, %0, %4\n"
"       strex   %1, %0, [%3]\n"
"       strex   %1, %0, [%3]\n"
"       teq     %1, #0\n"
"       bne     1b" /* the 2nd strex should fail, the whole program will hang */
        : "=&r" (result), "=&r" (tmp), "+Qo" (v->counter)
        : "r" (&v->counter), "Ir" (i)
        : "cc");
}


static int __init __atomic_init(void)
{
	__atomic_t a;

	printk("testing ldrex, strex....\n");
	__atomic_add(1, &a);
	__atomic_add_fail(1, &a);
	printk("tested ldrex, strex\n");

	__atomic_add_hang(1, &a);
	printk("never arrive here\n");

	return 0;
}
module_init(__atomic_init);
