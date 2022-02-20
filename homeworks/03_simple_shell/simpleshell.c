/* Copyright 2016 Rose-Hulman
   But based on idea from http://cnds.eecs.jacobs-university.de/courses/caoslab-2007/
   */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void kill_child(){
    int wait_time;
    wait(&wait_time);
    if (WEXITSTATUS(wait_time) != 0){
        printf("Command has finished\n");
    }
}
int run(char *parsed_command[2]){
    signal(SIGCHLD, kill_child);
    int shell = fork();
    if(shell < 0){
        perror("Fork has Failed.\n");
        return 1;
    }
    if(shell == 0){
        if(parsed_command[0][0] == 'B' && parsed_command[0][1] == 'G'){
            parsed_command[0] = parsed_command[0] + 2;
            int child = fork();
            if(child < 0){
                perror("Fork has Failed.\n");
                return 1;
            }
            if(child == 0){
                execlp(parsed_command[0], parsed_command[0], parsed_command[1], NULL);
            }
            int wait_time;
            wait(&wait_time);
            printf("Background command finished\n");
            exit(0);
        }
        else{
            execlp(parsed_command[0], parsed_command[0], parsed_command[1], NULL);
            int wait_time;
            wait(&wait_time);
            printf("Foreground command finished\n");
        }    
    }

}


int main() {
    char command[82];
    char *parsed_command[2];
    //takes at most two input arguments
    // infinite loop but ^C quits
    while (1) {
        printf("SHELL%% ");
        fgets(command, 82, stdin);
        command[strlen(command) - 1] = '\0';//remove the \n
        int len_1;
        for(len_1 = 0;command[len_1] != '\0';len_1++){
            if(command[len_1] == ' ')
                break;
        }
        parsed_command[0] = command;
        if(len_1 == strlen(command)){
            printf("Command is '%s' with no arguments\n", parsed_command[0]); 
            parsed_command[1] = NULL;
        }else{
            command[len_1] = '\0';
            parsed_command[1] = command + len_1 + 1;
            printf("Command is '%s' with argument '%s'\n", parsed_command[0], parsed_command[1]); 
        }
        run(parsed_command);
    }
}
