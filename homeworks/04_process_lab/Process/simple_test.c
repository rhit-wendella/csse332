#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>

#define simple_assert(message, test) do { if (!(test)) return message; } while (0)
#define TEST_PASSED NULL
#define DATA_SIZE 100
#define INITIAL_VALUE 77
#define MAX_TESTS 10

char* (*test_funcs[MAX_TESTS])(); // array of function pointers that store
                           // all of the tests we want to run
int num_tests = 0;

int data[DATA_SIZE][DATA_SIZE]; // shared data that the tests use

void add_test(char* (*test_func)()) {
    if(num_tests == MAX_TESTS) {
        printf("exceeded max possible tests");
        exit(1);
    }
    test_funcs[num_tests] = test_func;
    num_tests++;
}
// this setup function should run before each test
void setup() {
    printf("starting setup\n");
    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            data[i][j] = INITIAL_VALUE;
        }
    }
    // imagine this function does a lot of other complicated setup
    // that takes a long time
    usleep(3000000);
    
}

char* test1();

void snooze(){
    exit(2);
}


void run_all_tests() {
    setup();
    int run_order[num_tests];
    int pipeResult[num_tests][2];
    signal(SIGALRM, snooze);
    for(int i = 0; i < num_tests; i++){
        if(pipe(pipeResult[i])==-1){
            perror("BAD PIPE\n");
            exit(EXIT_FAILURE);
        }
        int child = fork();
        if(child == 0){
            alarm(3);
            char *tresult = test_funcs[i]();
            if(tresult == TEST_PASSED){
                close(pipeResult[i][1]);
                exit(0);
            }
            else {
                write(pipeResult[i][1], tresult, strlen(tresult));
                close(pipeResult[i][1]);
                exit(1);
            }
        }
        close(pipeResult[i][1]);
        run_order[i] = child;
    }
    for(int i = 0; i<num_tests; i++){
        int wait_time;
        waitpid(run_order[i], &wait_time, 0);
        char message[256];
        read(pipeResult[i][0], message, 256);
        if (!WIFEXITED(wait_time)){
            printf("TEST CRASHED\n");
        }
        else if(WEXITSTATUS(wait_time)==0){
            printf("TEST PASSED\n");
        }
        else if (WEXITSTATUS(wait_time) == 2){
            printf("TEST TIMED OUT\n");
        }
        else if (WEXITSTATUS(wait_time) == 1){
            printf("TEST FAILED because %s\n", message);
        }
    }   
}

char* test1() {

    printf("starting test 1\n");
    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            simple_assert("test 1 data not initialized properly", data[i][j] == INITIAL_VALUE);
        }
    }

    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            data[i][j] = 1;
        }
    }
    
    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            simple_assert("test 1 data not set properly", data[i][j] == 1);
        }
    }
    printf("ending test 1\n");
    return TEST_PASSED;
}

char* test2() {

    printf("starting test 2\n");
    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            simple_assert("test 2 data not initialized properly", data[i][j] == INITIAL_VALUE);
        }
    }

    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            data[i][j] = 2;
        }
    }

    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            simple_assert("test 2 data not set properly", data[i][j] == 2);
        }
    }

    printf("ending test 2\n");
    return TEST_PASSED;
}

char* test3() {

    printf("starting test 3\n");

    simple_assert("test 3 always fails", 1 == 2);
    
    printf("ending test 3\n");
    return TEST_PASSED;
}


char* test4() {

    printf("starting test 4\n");

    int *val = NULL;
    printf("data at val is %d", *val);
    
    printf("ending test 4\n");
    return TEST_PASSED;
}

char* test5() {

    printf("starting test 5\n");

    while(1) { } 
    
    printf("ending test 5\n");
    return TEST_PASSED;
}

void main() {
    add_test(test1);
    add_test(test2);
    add_test(test3);
    add_test(test4); // uncomment for Step 4
    add_test(test5); // uncomment for Step 5
    run_all_tests();
    
}
