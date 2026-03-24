#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#define r0 return 0
typedef double DBL;
typedef const double ADBL;

ADBL g = 9.81;

struct TPosition{
    DBL XI, YI, ZI;
};
struct TVelocity{
    DBL VX, VY, VZ;
};
struct Target{
    struct TPosition tpos;
    struct TVelocity tvel;
};

void segfaultrep(int signal){
    puts("Segfault occured. Inaccessible memory was attempted to be accessed");
    exit(EXIT_SUCCESS);
}

int main(int ARGC, char* ARG[]){
    signal(SIGSEGV,segfaultrep);
    struct Target T;
    struct Target* TPTR = &T;
    if (ARGC != 6){
        printf("6 args needed, %i were given.",ARGC);
        raise(SIGTERM);
    }
    TPTR->tpos.XI=atof(ARG[1]);
    TPTR->tpos.YI=atof(ARG[2]);
    TPTR->tpos.ZI=atof(ARG[3]);
    TPTR->tvel.VX=atof(ARG[4]);
    TPTR->tvel.VY=atof(ARG[5]);
    TPTR->tvel.VZ=atof(ARG[6]);
    
    r0;
}


  
