#include <linux/module.h>

static int rc = 0;

int init_bug_on(void)
{
        BUG_ON(rc == 0);
        return 0;
}

void exit_bug_on(void)
{
}

module_init(init_bug_on);
module_exit(exit_bug_on);
