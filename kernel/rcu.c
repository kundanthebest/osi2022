#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/kprobes.h>
#include<linux/spinlock.h>
#include<linux/slab.h>
#include<linux/rcupdate.h>

struct foo {
        int a;
        char b;
        long c;
};
DEFINE_SPINLOCK(foo_mutex);
struct foo __rcu *gbl_foo;
void foo_init_a(int new_a)
{
        struct foo *fp = NULL;
        fp = kmalloc(sizeof(*fp), GFP_KERNEL);
        fp->a = new_a;
        gbl_foo = fp;
}
void foo_update_a(int new_a)
{
        struct foo *new_fp;
        struct foo *old_fp;
        new_fp = kmalloc(sizeof(*new_fp), GFP_KERNEL);
        spin_lock(&foo_mutex);
        old_fp = rcu_dereference_protected(gbl_foo, lockdep_is_held(&foo_mutex));
        *new_fp = *old_fp;
        new_fp->a = new_a;
        rcu_assign_pointer(gbl_foo, new_fp);
        printk("updated pointer\n");
        spin_unlock(&foo_mutex);
        printk("synchronize rcu\n");
        synchronize_rcu();
        kfree(old_fp);
}
int foo_get_a(void)
{
        int retval;
        rcu_read_lock();
        retval = rcu_dereference(gbl_foo)->a;
        rcu_read_unlock();
        printk("%s, %d fetched val is %d\n", __func__, __LINE__, retval);
        return retval;
}
void foo_del_a(void)
{
        if(gbl_foo != NULL)
                kfree(gbl_foo);
}
int myinit(void)
{
    printk("module inserted\n");
    foo_init_a(70);
    foo_get_a();
    foo_get_a();
    foo_update_a(20);
    foo_get_a();
    foo_update_a(30);
    foo_get_a();
    foo_del_a();
    return 0;
}
void myexit(void)
{
    printk("module removed\n");
}
module_init(myinit);
module_exit(myexit);
MODULE_AUTHOR("K_K");
MODULE_DESCRIPTION("RCU MODULE");
MODULE_LICENSE("GPL");

