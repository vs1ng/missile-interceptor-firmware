#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#define r0 return 0
typedef double DBL;

struct Threat{
    DBL X0, Y0, Z0;
    DBL VX0, VY0, VZ0;
};

void problem(int sig){
    puts("Something went wrong.");
    exit(EXIT_FAILURE);
}

int main(int ARGC, char* ARG[]){
    signal(SIGSEGV, problem);
    struct Threat T;
    if (ARGC != 7){
        printf("Needed 7 args, got %i instead.\n",ARGC);
        raise(SIGTERM);
    T.VX0 = atof(ARG[1]);
    T.VY0 = atof(ARG[2]);
    T.VZ0 = atof(ARG[3]);
    T.X0 = atof(ARG[4]);
    T.Y0 = atof(ARG[5]);
    T.Z0 = atof(ARG[6]);
    r0;
}
