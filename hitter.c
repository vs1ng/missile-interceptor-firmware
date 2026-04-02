#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#define r0 return 0

void problem(int sig){
    puts("Something went wrong.");
    exit(EXIT_FAILURE);
}

int main(int ARGC, char* ARG[]){
    signal(SIGSEGV, problem);

    r0;
}
