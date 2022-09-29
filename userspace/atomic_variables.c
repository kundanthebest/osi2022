#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef unsigned int uint32_t;
typedef uint32_t uint32;
typedef volatile uint32 Atomic32;

Atomic32 x_atomic32_add(Atomic32 *atomic, Atomic32 val)
{
        return __sync_add_and_fetch(atomic, val);
}
Atomic32 counter = 0;
//int counter2 = 0;

//void *print_message_function( void *ptr );
void *print_message_function1( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);
    for (int i = 0;i < 100000; i++)
	__sync_add_and_fetch();
        x_atomic32_add(&counter, 1);
}

void *print_message_function2( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);
    for (int i = 0;i < 100000; i++)
        x_atomic32_add(&counter, 1);
        //counter2 ++;
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
     printf ("counter = %d\n",counter);
     //printf ("counter = %d\n",counter2);
     exit(0);
}

