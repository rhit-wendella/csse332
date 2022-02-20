#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int number = 1;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t three = PTHREAD_COND_INITIALIZER;
pthread_cond_t two = PTHREAD_COND_INITIALIZER;
pthread_cond_t four = PTHREAD_COND_INITIALIZER;

void *thread(void *arg)
{       
        int *num = (int *)arg;
        printf("%d wants to enter the critical section\n", *num);
        pthread_mutex_lock(&lock);
        while(number != *num){
                if(*num == 2){
                        pthread_cond_wait(&two, &lock);
                }
                else if(*num == 3){
                        pthread_cond_wait(&three, &lock);
                }
                else if(*num == 4){
                        pthread_cond_wait(&four, &lock);
                }
        }
        printf("%d is finished with the critical section\n", *num);
        number++;
        if(number == 2){
                pthread_cond_signal(&two);
        }
        else if(number == 3){
                pthread_cond_signal(&three);
        }
        else if(number == 4){
                pthread_cond_signal(&four);
        }
        pthread_mutex_unlock(&lock);
        return NULL;
}

int
main(int argc, char **argv)
{
        pthread_t threads[4];
        int i;
        int nums[] = {2, 1, 4, 3};

        for(i = 0; i < 4; ++i) {
                pthread_create(&threads[i], NULL, thread, &nums[i]);

                if(i == 2)
                        sleep(3);
        }

        for(i = 0; i < 4; ++i) {
                pthread_join(threads[i], NULL);
        }

        printf("Everything finished\n");

        return 0;
}