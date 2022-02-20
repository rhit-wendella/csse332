/* Copyright 2016 Rose-Hulman Institute of Technology

Here is some code that factors in a super dumb way.  We won't be
attempting to improve the algorithm in this case (though that would be
the correct thing to do).

Modify the code so that it starts the specified number of threads and
splits the computation among them.  You can be sure the max allowed 
number of threads is 50.  Be sure your threads actually run in parallel.

Your threads should each just print the factors they find, they don't
need to communicate the factors to the original thread.

ALSO - be sure to compile this code with -pthread or just used the 
Makefile provided.

 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

struct arg_struct{
  unsigned long long int start;
  unsigned long long int end;
  unsigned long long int target;
  unsigned long long int id;
};

void *runner(void* arguments) {
  struct arg_struct *arg = (struct arg_struct *)arguments;
  for (unsigned long long int i = arg->start; i <= arg->end; i = i + 1) {
    /* You'll want to keep this testing line in.  Otherwise it goes so
       fast it can be hard to detect your code is running in
       parallel. Also test with a large number (i.e. > 3000) */
    printf("Thread %llu testing %llu\n", arg->id, i);
    if (arg->target % i == 0) {
      printf("%llu is a factor\n", i);
    }
  }
}


int main(void) {
  /* you can ignore the linter warning about this */
  unsigned long long int target, i, start = 0;
  int numThreads;
  unsigned long long int increment;

  printf("Give a number to factor.\n");
  scanf("%llu", &target);

  printf("How man threads should I create?\n");
  scanf("%d", &numThreads);
  if (numThreads > 50 || numThreads < 1) {
    printf("Bad number of threads!\n");
    return 0;
  }

  pthread_t thread_id[numThreads];
  struct arg_struct arguments[numThreads];
  increment = target/numThreads;
  for (i = 1; i<=numThreads; i++){
    arguments[i-1].id = i;
    arguments[i-1].end = increment*i;
    arguments[i-1].target = target;
    arguments[i-1].start = 1 + (increment*(i-1));

    pthread_create(&thread_id[i-1], NULL, runner, (void *)&arguments[i-1]);
  }
  for (i = 0; i<numThreads; i++){
    pthread_join(thread_id[i], NULL);
  }


  return 0;
}

