#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>

#define BUF_SIZE (32*PAGE_SIZE)

static void *kbuff;

static int remap_pfn_open(struct inode *inode, struct file *file)
{
	struct mm_struct *mm = current->mm;
/*
    client: remap_pfn_test_ (707)
    code  section: [0x10000   0x6ad28]
    data  section: [0x7b000   0x7bf68]
    brk   section: s: 0x7d000, c: 0x9f000
    mmap  section: s: 0xb6fd1000
    stack section: s: 0xbeb8fe90
    arg   section: [0xbeb8ff6f   0xbeb8ff82]
    env   section: [0xbeb8ff82   0xbeb8ffe9]
*/
	printk("client: %s (%d)\n", current->comm, current->pid);
	printk("code  section: [0x%lx   0x%lx]\n", mm->start_code, mm->end_code);
	printk("data  section: [0x%lx   0x%lx]\n", mm->start_data, mm->end_data);
	printk("brk   section: s: 0x%lx, c: 0x%lx\n", mm->start_brk, mm->brk);
	printk("mmap  section: s: 0x%lx\n", mm->mmap_base);
	printk("stack section: s: 0x%lx\n", mm->start_stack);
	printk("arg   section: [0x%lx   0x%lx]\n", mm->arg_start, mm->arg_end);
	printk("env   section: [0x%lx   0x%lx]\n", mm->env_start, mm->env_end);

	return 0;
}

static int remap_pfn_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
/*
    计算内核缓冲区中将被映射到用户空间的地址对应的物理页帧号。
*/
	unsigned long pfn_start = (virt_to_phys(kbuff) >> PAGE_SHIFT) + vma->vm_pgoff;
/*
    计算内核缓冲区中将被映射到用户空间的地址对应的虚拟地址	
*/
	unsigned long virt_start = (unsigned long)kbuff + offset;
/*
    计算该vma表示的内存区间的大小	
*/
	unsigned long size = vma->vm_end - vma->vm_start;
	int ret = 0;

/*
    vm_pgoff: 0x0
*/
    printk("vm_pgoff: 0x%lx\n", vma->vm_pgoff);
    
/*

    phy: 0x8dc80000, offset: 0x0, size: 0x10000
*/
	printk("phy: 0x%lx, offset: 0x%lx, size: 0x%lx\n", pfn_start << PAGE_SHIFT, offset, size);
/*
    remap_pfn_mmap: map 0xedc80000 to 0xb6fc1000, size: 0x10000
    将物理页帧号pfn_start对应的物理内存映射到用户空间的vm->vm_start处，映射长度为该虚拟内存区的长度。
    由于这里的内核缓冲区是用kzalloc分配的，保证了物理地址的连续性，
    所以会将物理页帧号从pfn_start开始的（size >> PAGE_SHIFT）个连续的物理页帧依次按序映射到用户空间
*/
	ret = remap_pfn_range(vma, vma->vm_start, pfn_start, size, vma->vm_page_prot);
	if (ret)
		printk("%s: remap_pfn_range failed at [0x%lx  0x%lx]\n",
			__func__, vma->vm_start, vma->vm_end);
	else
		printk("%s: map 0x%lx to 0x%lx, size: 0x%lx\n", __func__, virt_start,
			vma->vm_start, size);

	return ret;
}

static const struct file_operations remap_pfn_fops = {
	.owner = THIS_MODULE,
	.open = remap_pfn_open,
	.mmap = remap_pfn_mmap,
};

static struct miscdevice remap_pfn_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "remap_pfn",
	.fops = &remap_pfn_fops,
};

static int __init remap_pfn_init(void)
{
	int ret = 0;

	kbuff = kzalloc(BUF_SIZE, GFP_KERNEL);
	if (!kbuff) {
		ret = -ENOMEM;
		goto err;
	}
/*
    0xedc80000	
*/
    printk("kbuff: 0x%lx\n", kbuff);

	ret = misc_register(&remap_pfn_misc);
	if (unlikely(ret)) {
		pr_err("failed to register misc device!\n");
		goto err;
	}

	return 0;

err:
	return ret;
}

static void __exit remap_pfn_exit(void)
{
	misc_deregister(&remap_pfn_misc);
	kfree(kbuff);
}

module_init(remap_pfn_init);
module_exit(remap_pfn_exit);
MODULE_LICENSE("GPL");
