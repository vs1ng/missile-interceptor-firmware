#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#define r0 return 0
typedef double DBL;
typedef const double ADBL;

ADBL g = 9.81;
ADBL obt = 0.5;

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
            "movsd xmm0, %[vx]\n\t"
            "movsd xmm1, %[t]\n\t"
            "mulsd xmm0, xmm1\n\t"
            "movsd xmm1, %[xi]\n\t"
            "addsd xmm0, xmm1\n\t"
            "movsd xmm0, %[XT]\n\t"
            : [XT] "=m" (XT)
            : [vx] "m" (vx), [t] "m" (t), [xi] "m" (xi)
       
            : "xmm0", "xmm1", "memory"
    );
    asm volatile(
            "movsd xmm0, %[vy]\n\t"
            "movsd xmm1, %[t]\n\t"
            "mulsd xmm0, xmm1\n\t"
            "movsd xmm1, %[yi]\n\t"
            "addsd xmm0, xmm1\n\t"
            "movsd xmm0, %[YT]\n\t"
            : [YT] "=m" (YT)
            : [vy] "m" (vy), [t] "m" (t), [yi] "m" (yi)
            : "xmm1", "xmm0", "memory"
     );
    asm volatile(
            "movsd xmm0, %[vz]\n\t"
            "movsd xmm1, %[t]\n\t"
            "mulsd xmm0, xmm1\n\t"
            "movsd xmm1, %[zi]\n\t"
            "addsd xmm0, xmm1\n\t"

            "movsd xmm2, %[t]\n\t"
            "mulsd xmm2, xmm2\n\t"
            "movsd xmm3, %[g]\n\t"
            "mulsd xmm2, xmm3\n\t"
            "movsd xmm3, %[obt]\n\t"
            "mulsd xmm2, xmm3\n\t"

            "subsd xmm0,xmm2\n\t"

            "movsd xmm0, %[ZT]\n\t"
            : [ZT] "=m" (ZT)
            : [vz] "m" (vz), [t] "m" (t), [zi] "m" (zi), [g] "m" (g), [obt] "m" (obt)
            : "xmm1" , "xmm0", "memory"
     );

    r0;
}


  
