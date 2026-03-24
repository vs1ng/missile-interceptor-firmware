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
    DBL vx,vy,vz,xi,yi,zi,t=2;
    xi = TPTR->tpos.XI = atof(ARG[1]);
    yi = TPTR->tpos.YI = atof(ARG[2]);
    zi = TPTR->tpos.ZI = atof(ARG[3]);
    vx = TPTR->tvel.VX = atof(ARG[4]);
    vy = TPTR->tvel.VY = atof(ARG[5]);
    vz = TPTR->tvel.VZ = atof(ARG[6]);
    DBL XT,YT,ZT;
    asm volatile(
            "mov eax, %[vx]\n\t"
            "imul eax,%[t]\n\t"
            "add eax,%[xi]\n\t"
            "mov %[XT],eax\n\t"
            : [XT] "+r" (XT)
            : [vx] "r" (vx), 
              [t] "r" (t),
              [xi] "r" (xi)
            : "eax"
    );


    r0;
}


  
