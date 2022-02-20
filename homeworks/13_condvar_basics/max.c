#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int number = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait = PTHREAD_COND_INITIALIZER;

void *thread(void *arg)
{
	char *letter = (char *)arg;
	printf("%c wants to enter the critical section\n", *letter);
	if(number>=3){
		pthread_mutex_lock(&lock);
	}
	while(number>=3){
		pthread_cond_wait(&wait, &lock);
	}
	printf("%c is in the critical section\n", *letter);
	number++;
	sleep(1);
	printf("%c has left the critical section\n", *letter);
	number--;
	pthread_cond_signal(&wait);
	pthread_mutex_unlock(&lock);
	return NULL;
}

int
main(int argc, char **argv)
{
	pthread_t threads[8];
	int i;
	char *letters = "abcdefgh";

	for(i = 0; i < 8; ++i) {
		pthread_create(&threads[i], NULL, thread, &letters[i]);

		if(i == 4)
			sleep(4);
	}

	for(i = 0; i < 8; i++) {
		pthread_join(threads[i], NULL);
	}

	printf("Everything finished...\n");

	return 0;
}
