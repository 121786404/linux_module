#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/ctype.h>
#include <linux/kobject.h>
#include <linux/genhd.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kernel_stat.h>
#include <linux/errno.h>
#include <linux/mmzone.h>
#include <linux/suspend.h>
#include <asm/uaccess.h>
#include <linux/kthread.h>
#include <linux/freezer.h>

static int pgdatinfo_init(void)
{
	pg_data_t *pgdat;

	printk("pgdat:");
	for_each_online_pgdat(pgdat) {
		printk(" %d: %lu,%lu,%lu",
				pgdat->node_id,
				pgdat->node_start_pfn,
				pgdat->node_present_pages,
				pgdat->node_spanned_pages);
#ifdef CONFIG_FLAT_NODE_MEM_MAP
		printk(",%p", pgdat->node_mem_map);
#endif
	}
	printk("\n");
	return 0;
}

static void __exit pgdatinfo_exit(void)
{
}

MODULE_AUTHOR("SWsoft <info@sw-soft.com>");
MODULE_DESCRIPTION("Virtuozzo WDOG");
MODULE_LICENSE("GPL v2");

module_init(pgdatinfo_init)
module_exit(pgdatinfo_exit)