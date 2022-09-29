#include <urcu/urcu-mb.h>
#include <urcu/urcu-bp.h>
#include <urcu/urcu-memb.h>
#include <urcu/urcu-signal.h>
#include <urcu/urcu-qsbr.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct foo {
        int a;
        char b;
        long c;
};

struct foo __rcu *gbl_foo;
int *global_ptr = NULL;

void foo_init_a(int new_a)
{
        struct foo *fp = NULL;
        fp = malloc(sizeof(*fp));
        fp->a = new_a;
        gbl_foo = fp;
}
void foo_update_a(int new_a)
{
#if 0
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
#endif
}
int foo_get_a(void)
{
        int retval;
        urcu_qsbr_read_lock();
        retval = rcu_dereference(gbl_foo)->a;
        urcu_qsbr_read_unlock();
        //printf("%s, %d fetched val is %d\n", __func__, __LINE__, retval);
        return retval;
}
void foo_del_a(void)
{
        if(gbl_foo != NULL)
                free(gbl_foo);
}


void *print_message_function1( void *ptr )
{
     char *message;
     volatile int a;
     message = (char *) ptr;
     printf("%s \n", message);
     urcu_qsbr_register_thread();

     for (int i = 0;i < 100000; i++)
     {
	     a = foo_get_a();
     }
     urcu_bp_quiescent_state();
     urcu_qsbr_unregister_thread();
}

void *print_message_function2( void *ptr )
{
	char *message;
	volatile int a;
	message = (char *) ptr;
	printf("%s \n", message);
	urcu_qsbr_register_thread();
	for (int i = 0;i < 100000; i++)
	{
		a = foo_get_a();
	}
	urcu_bp_quiescent_state();
	urcu_qsbr_unregister_thread();
}


static int test_mf_qsbr(void)
{
	urcu_qsbr_register_thread();
	urcu_qsbr_read_lock();
	urcu_qsbr_read_unlock();
	urcu_qsbr_synchronize_rcu();
	urcu_qsbr_unregister_thread();
	return 0;
}

int main()
{
	pthread_t thread1, thread2;
	char *message1 = "Thread 1";
	char *message2 = "Thread 2";
	int  iret1, iret2;

	/* Create independent threads each of which will execute function */
	struct timeval stop, start;
	double secs = 0;
	gettimeofday(&start, NULL);
	iret1 = pthread_create( &thread1, NULL, print_message_function1, (void*) message1);
	iret2 = pthread_create( &thread2, NULL, print_message_function2, (void*) message2);

	foo_init_a(10);

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */

	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL);
	gettimeofday(&stop, NULL);
	//printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
	secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
	printf("time taken %f\n",secs);
	printf("Thread 1 returns: %d\n",iret1);
	printf("Thread 2 returns: %d\n",iret2);
	//printf ("counter = %d\n",counter);
	//printf ("counter = %d\n",counter2);
	foo_del_a();
	test_mf_qsbr();
	return 0;
}
