# remap_pfn_demo

学习remap_pfn_range的例子。
    在驱动中申请一个32个Page的缓冲区，这里的PAGE_SIZE是4KB，所以内核中的缓冲区大小是128KB。user_1和user_2将前64KB映射到自
己的用户空间，其中user_1向缓冲区中写入字符串，user_2去读取。user_3和user_4将后64KB映射到自己的用户空间，其中user_3向缓冲区
中写入字符串，user_4读取字符串。user_5将整个128KB映射到自己的用户空间，然后将缓冲区清零。
    此外，在驱动中申请缓冲区的方式有多种，可以用kmalloc、也可以用alloc_pages，当然也可用vmalloc，下面会分别针对这三个接口实
现驱动。
