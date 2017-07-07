/*
 * ...
 * print_hex_dump_bytes is EXPORTed by linux/lib/hexdump.c
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YKR");

//extern char __initdata boot_command_line[];
//extern char * saved_command_line;

static int __init phd_init(void)
{
    pr_info("Hello, print_hex_dump_module.\n");
    pr_info("boot_command_line:\n");
    print_hex_dump_bytes("", DUMP_PREFIX_NONE, boot_command_line, strlen(boot_command_line));
    pr_info("\nsaved_command_line:\n");
    print_hex_dump_bytes("", DUMP_PREFIX_NONE, saved_command_line, strlen(saved_command_line));
    pr_info("\n-- command_line end --\n");
    pr_info("dump_stack:\n");
    dump_stack();
    pr_info("-- dump_stack end --\n");
    return 0;
}

static void __exit phd_exit(void)
{
    pr_info("Goodbye, print_hex_dump_module.\n");
}

module_init(phd_init);
module_exit(phd_exit);
