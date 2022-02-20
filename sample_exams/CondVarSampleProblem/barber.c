#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CUSTOMERS 10
#define MAX_CHAIRS 4

int chairs = 0;
int service_chair = 0;

pthread_cond_t barber_wait, customer_wait, service_wait;
pthread_cond_t barber_wait = PTHREAD_COND_INITIALIZER;
pthread_cond_t customer_wait = PTHREAD_COND_INITIALIZER;
pthread_cond_t service_wait = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock;

void*
barber_fn(void *arg)
{
	printf("The barber is now in the shop...\n");

	while(1) {
		pthread_mutex_lock(&lock);
		while(service_chair == 0){
			pthread_cond_wait(&barber_wait, &lock);
		}
		pthread_mutex_unlock(&lock);

		// start cutting hair
		printf("Barber cutting customer hair\n");
		sleep(1);
		pthread_mutex_lock(&lock);
		pthread_cond_signal(&service_wait);
		service_chair = 0;
		pthread_mutex_unlock(&lock);
	}
}

void*
customer_fn(void *arg)
{
	int added = 0;
	printf("New customer arrived...\n");
	pthread_mutex_lock(&lock);
	if(chairs>=4){
		pthread_mutex_unlock(&lock);
		printf("Customer leaving because chairs are all occupied..\n");
		return NULL;
	}
	while(service_chair == 1){
		if(!added){
			printf("Customer waiting in the waiting chair...\n");
			chairs++;
			added = 1;
		}
		pthread_cond_wait(&customer_wait, &lock);
	}
	pthread_mutex_unlock(&lock);
	printf("customer ready to sit in the barber chair\n");	
	pthread_mutex_lock(&lock);
	if(added){
		chairs--;
	}
	service_chair = 1;
	pthread_cond_signal(&barber_wait);

	pthread_cond_wait(&service_wait, &lock);
	printf("Customer done with the haircut...\n");
	pthread_cond_signal(&customer_wait);
	pthread_mutex_unlock(&lock);

	return NULL;
}

int
main(int argc, char **argv)
{
	int i;
	pthread_t barber;
	pthread_t customers[NUM_CUSTOMERS];
	pthread_mutex_init(&lock, NULL);

	pthread_create(&barber, 0, barber_fn, 0);
	for(i = 0; i < NUM_CUSTOMERS; ++i) {
		pthread_create(&customers[i], 0, customer_fn, 0);

		if(i==6) sleep(1);
	}

	for(i = 0; i < NUM_CUSTOMERS; ++i) {
		pthread_join(customers[i], 0);
	}

	printf("All customers finished, barber shop closed...\n");
}
