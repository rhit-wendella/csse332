/* Copyright 2019 Rose-Hulman */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


// number of carpenters
#define NUM_CARP 3
// number of painters
#define NUM_PAIN 3
// number of decorators
#define NUM_DECO 3

/**
  Imagine there is a shared memory space called house.

  There are 3 different kinds of operations on house: carpenters, painters, and
  decorators.  For any particular kind of operation, there can be an unlimited
  number of threads doing the same operation at once (e.g. unlimited carpenter
  threads etc.).  However, only one kind of operation can be done at a time (so
  even a single carpenter should block all painters and vice versa).

  Use mutex locks and condition variables to enforce this constraint.  You don't
  have to worry about starvation (e.g. that constantly arriving decorators might
  prevent carpenters from ever running) - though maybe it would be fun to
  consider how you would solve in that case.

  This is similar to the readers/writers problem BTW.
 **/
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t carpen = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t paint = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t decor = PTHREAD_MUTEX_INITIALIZER;

int carpen_number = 0;
int paint_number = 0;
int decor_number = 0;

void* carpenter(void * ignored) {
	pthread_mutex_lock(&lock);
	if(paint_number<=0 && decor_number<=0){
		pthread_mutex_unlock(&carpen);
	}
	carpen_number++;
	pthread_mutex_unlock(&lock);
	pthread_mutex_lock(&carpen);
	printf("starting carpentry\n");
	sleep(1);
	printf("finished carpentry\n");
	pthread_mutex_lock(&lock);
	carpen_number--;
	if(carpen_number>0){
		pthread_mutex_unlock(&carpen);
	}
	else if(paint_number>0){
		pthread_mutex_unlock(&paint);
	}
	else if(decor_number>0){
		pthread_mutex_unlock(&decor);
	}
	pthread_mutex_unlock(&lock);
	return NULL;
}

void* painter(void * ignored) {
	pthread_mutex_lock(&lock);
	if(carpen_number<=0 && decor_number<=0){
		pthread_mutex_unlock(&paint);
	}
	paint_number++;
	pthread_mutex_unlock(&lock);
	pthread_mutex_lock(&paint);
	printf("starting painting\n");
	sleep(1);
	printf("finished painting\n");
	pthread_mutex_lock(&lock);
	paint_number--;
	if(paint_number>0){
		while(paint_number>0){
			pthread_mutex_unlock(&paint);
			paint_number--;
		}
	}
	else if(carpen_number>0){
		pthread_mutex_unlock(&carpen);
	}
	else if(decor_number>0){
		pthread_mutex_unlock(&decor);
	}
	pthread_mutex_unlock(&lock);
	return NULL;
}

void* decorator(void * ignored) {
	pthread_mutex_lock(&lock);
	if(carpen_number<=0 && paint_number<=0){
		pthread_mutex_unlock(&decor);
	}
	decor_number++;
	pthread_mutex_unlock(&lock);
	pthread_mutex_lock(&decor);
	printf("starting decorating\n");
	sleep(1);
	printf("finished decorating\n");
	pthread_mutex_lock(&lock);
	decor_number--;
	if(decor_number>0)
        while (decor_number>0){
            pthread_mutex_unlock(&decor);
            decor_number--;
        }
    else if(carpen_number>0){
        pthread_mutex_unlock(&carpen);
	}
    else if(paint_number>0){
		pthread_mutex_unlock(&paint);
	}
	pthread_mutex_unlock(&lock);
	return NULL;
}


int main(int argc, char **argv) {
	pthread_t jobs[NUM_CARP + NUM_PAIN + NUM_DECO];
	for (int i = 0; i < NUM_CARP + NUM_PAIN + NUM_DECO; i++) {
		void* (*func) (void*) = NULL;
		if(i < NUM_CARP)
			func = carpenter;
		if(i >= NUM_CARP && i < NUM_CARP + NUM_PAIN)
			func = painter;
		if(i >= NUM_CARP + NUM_PAIN) {
			func = decorator;
		}
		pthread_create(&jobs[i], NULL, func, NULL);
	}

	for (int i = 0; i < NUM_CARP + NUM_PAIN + NUM_DECO; i++) {
		pthread_join(jobs[i], NULL);
	}

	printf("Everything finished.\n");

}
