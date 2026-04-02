#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#define r0 return 0
typedef double DBL;
typedef const double CD;

CD g = 9.18;
CD dt = 0.01;
CD N = 4;
CD burn_time = 3.0;
CD mass_i = 80;
CD fuel_mass = 20;
CD thrust = 4000;
CD rho = 1.225;
CD Cd = 0.5;
CD A = 0.02;
const char hit_r = 3;
DBL max_turn = (30)*(g);
DBL fuel_rt = fuel_mass/burn_time;
DBL dry_mass = mass_i-fuel_mass;


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
    DBL t = 2;

    r0;
}
