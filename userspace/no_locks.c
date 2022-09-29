#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

volatile int counter = 0;
void *print_message_function1( void *ptr )
{
	char *message;
	message = (char *) ptr;
	printf("%s \n", message);
	for (int i = 0;i < 100000; i++)
		counter = counter + 1;
}

void *print_message_function2( void *ptr )
{
	char *message;
	message = (char *) ptr;
	printf("%s \n", message);
	for (int i = 0;i < 100000; i++)
		counter = counter + 1;
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

	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL); 
	gettimeofday(&stop, NULL);
	secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
	printf("time taken %f secs\n",secs);
	printf("Thread 1 returns: %d\n",iret1);
	printf("Thread 2 returns: %d\n",iret2);
	printf ("counter = %d\n",counter);
	exit(0);
}

