#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#define r0 return 0
typedef double DBL;
typedef const double ADBL;

ADBL g = 9.81;
ADBL obt = 0.5;
ADBL mass0 = 90; 
ADBL fuelBurnRT = 2.3;
ADBL rho = 1.225;
ADBL Cl = 0.5;
ADBL Cd = 0.5;
ADBL A = 0.02;
char burnTime = 5;

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
    if (ARGC != 7){
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
        "movsd %[vx], %%xmm0\n\t"
        "movsd %[t], %%xmm1\n\t"
        "mulsd %%xmm1, %%xmm0\n\t"
        "movsd %[xi], %%xmm1\n\t"
        "addsd %%xmm1, %%xmm0\n\t"
        "movsd %%xmm0, %[XT]\n\t"
        : [XT] "=m" (XT)
        : [vx] "m" (vx), [t] "m" (t), [xi] "m" (xi)
        : "xmm0", "xmm1"
    );

    asm volatile(
        "movsd %[vy], %%xmm0\n\t"
        "movsd %[t], %%xmm1\n\t"
        "mulsd %%xmm1, %%xmm0\n\t"
        "movsd %[yi], %%xmm1\n\t"
        "addsd %%xmm1, %%xmm0\n\t"
        "movsd %%xmm0, %[YT]\n\t"
        : [YT] "=m" (YT)
        : [vy] "m" (vy), [t] "m" (t), [yi] "m" (yi)
        : "xmm0", "xmm1"
    );

    asm volatile(
        "movsd %[vz], %%xmm0\n\t"
        "movsd %[t], %%xmm1\n\t"
        "mulsd %%xmm1, %%xmm0\n\t"      
        "movsd %[zi], %%xmm1\n\t"
        "addsd %%xmm1, %%xmm0\n\t"      
        "movsd %[t], %%xmm1\n\t"
        "mulsd %%xmm1, %%xmm1\n\t"     
        "movsd %[g], %%xmm2\n\t"
        "mulsd %%xmm2, %%xmm1\n\t"      
        "movsd %[obt], %%xmm2\n\t"
        "mulsd %%xmm2, %%xmm1\n\t"      
        "subsd %%xmm1, %%xmm0\n\t"      
        "movsd %%xmm0, %[ZT]\n\t"
        : [ZT] "=m" (ZT)
        : [vz] "m" (vz), [t] "m" (t), [zi] "m" (zi), 
          [g] "m" (g), [obt] "m" (obt)
        : "xmm0", "xmm1", "xmm2"
    );
    printf("Phase 1: Target X:\040%.2f\tY:\040%.2f\tZ:\040%.2f\t\n",XT,YT,ZT);
    DBL rx = XT; DBL ry = YT; DBL rz = ZT;
    DBL DIST = 0;
    asm volatile(
            "movsd %[rx], %%xmm0\n\t"
            "mulsd %%xmm0, %%xmm0\n\t"
            "movsd %[ry], %%xmm1\n\t"
            "mulsd %%xmm1, %%xmm1\n\t"
            "movsd %[rz], %%xmm2\n\t"
            "mulsd %%xmm2, %%xmm2\n\t"
            "addsd %%xmm0, %%xmm1\n\t"
            "addsd %%xmm0, %%xmm2\n\t"
            "sqrtsd %%xmm0, %%xmm0\n\t"
            "movsd %%xmm0, %[DIST]\n\t"
            : [DIST] "=m" (DIST)
            : [rx] "m" (rx), [ry] "m" (ry), [rz] "m" (rz)
            : "xmm0", "xmm1", "xmm2"
    );
    printf("Phase 2.1: Distance to Target = %.2f\n",DIST);
    DBL REQSPD;
    asm volatile(
        "movsd %[DIST], %%xmm0\n\t"
        "movsd %[t], %%xmm1\n\t"
        "divsd %%xmm1, %%xmm0\n\t"
        "movsd %%xmm0, %[REQSPD]\n\t"
        : [REQSPD] "=m" (REQSPD)
        : [DIST] "m" (DIST), [t] "m" (t)
        : "xmm0", "xmm1"
    );
    printf("Phase 2.2: Required Speed = %.2f\n",REQSPD);
    DBL dirx, diry, dirz;
    asm volatile(
        "movsd %[rx], %%xmm0\n\t"
        "movsd %[ry], %%xmm1\n\t"
        "movsd %[rz], %%xmm2\n\t"
        "movsd %[DIST], %%xmm3\n\t"
        "divsd %%xmm3, %%xmm0\n\t"
        "divsd %%xmm3, %%xmm1\n\t"
        "divsd %%xmm3, %%xmm2\n\t"
        "movsd %%xmm0, %[dirx]\n\t"
        "movsd %%xmm1, %[diry]\n\t"
        "movsd %%xmm2, %[dirz]\n\t"
        : [dirx] "=m" (dirx), [diry] "=m" (diry), [dirz] "=m" (dirz)
        : [rx] "m" (rx), [ry] "m" (ry), [rz] "m" (rz), [DIST] "m" (DIST)
        : "xmm0", "xmm1", "xmm2", "xmm3" 
    );
    printf("Phase 2.3: Direction Vector: %.2fi+%.2fj+%.2fk\n",dirx,diry,dirz);
    DBL VIX, VIY, VIZ;
    asm volatile(
        "movsd %[dirx], %%xmm0\n\t"
        "movsd %[diry], %%xmm1\n\t"
        "movsd %[dirz], %%xmm2\n\t"
        "movsd %[REQSPD], %%xmm3\n\t"
        "mulsd %%xmm3, %%xmm0\n\t"
        "mulsd %%xmm3, %%xmm1\n\t"
        "mulsd %%xmm3, %%xmm2\n\t"
        "movsd %%xmm0, %[VIX]\n\t"
        "movsd %%xmm1, %[VIY]\n\t"
        "movsd %%xmm2, %[VIZ]\n\t"
        : [VIX] "=m" (VIX), [VIY] "=m" (VIY), [VIZ] "=m" (VIZ)
        : [dirx] "m" (dirx), [diry] "m" (diry), [dirz] "m" (dirz), [REQSPD] "m" (REQSPD)
        : "xmm0", "xmm1", "xmm2", "xmm3"
    );
    printf("Phase 2.4: Interceptor Velocity Components: (%.2f, %.2f, %.2f)\n",VIX,VIY,VIZ);
    DBL AX, AY, AZ;
    asm volatile(
        "movsd %[VIX], %%xmm0\n\t"
        "movsd %[VIY], %%xmm1\n\t"
        "movsd %[VIZ], %%xmm2\n\t"
        "movsd %[t], %%xmm3\n\t"
        "movsd %[g], %%xmm4\n\t"
        "divsd %%xmm3, %%xmm0\n\t"
        "divsd %%xmm3, %%xmm1\n\t"
        "divsd %%xmm3, %%xmm2\n\t"
        "addsd %%xmm4, %%xmm2\n\t"
        "movsd %%xmm0, %[AX]\n\t"
        "movsd %%xmm1, %[AY]\n\t"
        "movsd %%xmm2, %[AZ]\n\t"
        : [AX] "=m" (AX), [AY] "=m" (AY), [AZ] "=m" (AZ)
        : [VIX] "m" (VIX), [VIY] "m" (VIY), [VIZ] "m" (VIZ), [t] "m" (t), [g] "m" (g)
        : "memory"
    );
    printf("Phase 2.5: Interceptor Accelertion: (%.2f, %.2f, %.2f)\n",AX,AY,AZ);
    DBL mass_current, a_thrust, tt, dt = 0.01;
    char HIT = 1;
    while(HIT){
        asm volatile(
                "movsd %[tt], %%xmm0\n\t"
                "movsd %[dt], %%xmm1\n\t"
                "addsd %%xmm1, %%xmm0\n\t"
                "movsd %%xmm0, %[tt]\n\t"
                : [tt] "=m" (tt)
                : [dt] "m" (dt) 
                : "xmm0", "xmm1"
        );
        if ( t < burnTime ){
            DBL mass;
            asm volatile(
                "movsd %[mass0], %%xmm0\n\t"
                "movsd %[fuelBurnRT], %%xmm1\n\t"
                "movsd %[tt], %%xmm2\n\t"
                "mulsd %%xmm1, %%xmm2\n\t"
                "subsd %%xmm2, %%xmm0\n\t"
                "movsd %%xmm0, %[mass]\n\t"
                : [mass] "=m" (mass)
                : [mass0] "m" (mass0), [fuelBurnRT] "m" (fuelBurnRT), [tt] "m" (tt)
                : "xmm0", "xmm1", "xmm2"
            );
            DBL VM;
            asm volatile(
                "movsd %[VIX], %%xmm0\n\t"
                "movsd %[VIY], %%xmm1\n\t"
                "movsd %[VIZ], %%xmm2\n\t"
                "mulsd %%xmm0, %%xmm0\n\t"
                "mulsd %%xmm1, %%xmm1\n\t"
                "mulsd %%xmm2, %%xmm2\n\t"
                "addsd %%xmm1, %%xmm0\n\t"
                "addsd %%xmm2, %%xmm0\n\t"
                "sqrtsd %%xmm0, %%xmm0\n\t"
                "movsd %%xmm0, %[VM]\n\t"
                : [VM] "=m" (VM)
                : [VIX] "m" (VIX), [VIY] "m" (VIY), [VIZ] "m" (VIZ)
                : "memory"
            );
            DBL fx, fy, fz;
            asm volatile(
                "movsd %[VM], %%xmm0\n\t"
                "movsd %[VIX], %%xmm1\n\t"
                "divsd %%xmm0, %%xmm1\n\t"
                "movsd %[VIY], %%xmm2\n\t"
                "divsd %%xmm0, %%xmm2\n\t"
                "movsd %[VIZ], %%xmm3\n\t"
                "divsd %%xmm0, %%xmm3\n\t"
                "movsd %%xmm1, %[fx]\n\t"
                "movsd %%xmm2, %[fy]\n\t"
                "movsd %%xmm3, %[fz]\n\t"
                : [fx] "=m" (fx), [fy] "=m" (fy), [fz] "=m" (fz)
                : [VM] "m" (VM), [VIX] "m" (VIX), [VIY] "m" (VIY), [VIZ] "m" (VIZ)
                : "memory"
           );
           DBL Mach;
           asm volatile(
                "movsd %[VM], %%xmm0\n\t"
                "movsd 343,%%xmm1\n\t"
                "divsd %%xmm1, %%xmm0\n\t"
                "movsd %%xmm0, %[Mach]\n\t"
                : [Mach] "=m" (Mach)
                : [VM] "m" (VM)
                : "xmm0", "xmm1"
           );
           DBL Fdrag;
           asm volatile(
                "movsd %[rho], %%xmm0\n\t"
                "movsd %[Cd], %%xmm1\n\t"
                "movsd %[A], %%xmm2\n\t"
                "movsd %[VM], %%xmm3\n\t"
                "mulsd %%xmm1, %%xmm0\n\t"
                "mulsd %%xmm2, %%xmm0\n\t"
                "mulsd %%xmm3, %%xmm3\n\t"
                "mulsd %%xmm3, %%xmm0\n\t"
                "movsd %[obt], %%xmm1\n\t"
                "mulsd %%xmm1, %%xmm0\n\t"
                "movsd %%xmm0, %[Fdrag]\n\t"
                : [Fdrag] "=m" (Fdrag)
                : [rho] "m" (rho), [Cd] "m" (Cd), [A] "m" (A), [VM] "m" (VM), [obt] "m" (obt)
                : "memory"
           );
           DBL adrag;
           DBL MC = mass_current;
           asm volatile(
               "movsd %[Fdrag], %%xmm0\n\t"
               "movsd %[MC], %%xmm1\n\t"
               "movsd %[VM], %%xmm2\n\t"
               "movsd 0xFFFF, %%xmm3\n\t"
               "mulsd %%xmm3, %%xmm0\n\t"
               "divsd %%xmm1, %%xmm0\n\t"
               "mulsd %%xmm0, %%xmm2\n\t"
               "movsd %%xmm2, %[adrag]\n\t"
               : [adrag] "=m" (adrag)
               : [Fdrag] "m" (Fdrag), [MC] "m" (MC), [VM] "m" (VM)
               : "memory"
            );
           DBL Flift, alift;
           asm volatile(
               "movsd %[obt], %%xmm0\n\t"
               "movsd %[rho], %%xmm1\n\t"
               "movsd %[A], %%xmm2\n\t"
               "movsd %[VM], %%xmm3\n\t"
               "movsd %[Cl], %%xmm4\n\t"
               "mulsd %%xmm3, %%xmm3\n\t"
               "mulsd %%xmm4, %%xmm3\n\t"
               "mulsd %%xmm2, %%xmm3\n\t"
               "mulsd %%xmm1, %%xmm3\n\t"
               "mulsd %%xmm0, %%xmm3\n\t"
               "movsd %%xmm3, %[Flift]\n\t"
               "xorsd %%xmm0, %%xmm0\n\t"
               "xorsd %%xmm1, %%xmm1\n\t"
               "xorsd %%xmm2, %%xmm2\n\t"
               "xorsd %%xmm4, %%xmm4\n\t"
               "movsd %[MC], %%xmm2\n\t"
               "divsd %%xmm2, %%xmm3\n\t"
               "movsd %%xmm3, %[alift]\n\t"
               : [Flift] "=m" (Flift), [alift] "=m" (alift)
               : [obt] "m" (obt), [rho] "m" (rho), [A] "m" (A), [VM] "m" (VM), [Cl] "m" (Cl), [MC] "m" (MC)
               : "memory"
            );
          DBL gamma = M_PI/2;
          DBL Integrated_V = (((Flift - Fdrag)/MC)-(g)*(sin(gamma))*t);
          DBL Integrated_Y = ((Flift/(MC*VM))-(g/VM)*(cos(gamma)));
          DBL Integrated_X = VM*cos(gamma);
          DBL Integrated_Z = VM*sin(gamma);
          DBL anet = 0; DBL dt = t; DBL athr = a_thrust;  
          asm volatile(
              "movsd %[VM], %%xmm0\n\t"
              "movsd %[alift], %%xmm1\n\t"
              "movsd %[adrag], %%xmm2\n\t"
              "movsd %[athr], %%xmm3\n\t"
              "addsd %%xmm1, %%xmm4\n\t"
              "addsd %%xmm2, %%xmm4\n\t"
              "addsd %%xmm3, %%xmm4\n\t"
              "movsd %[dt], %%xmm5\n\t"
              "mulsd %%xmm5, %%xmm4\n\t"
              "addsd %%xmm4, %%xmm0\n\t"
              "movsd %%xmmm0, %[VM]\n\t"
              : [VM] "=m" (VM)
              : [alift] "m" (alift), [adrag] "m" (adrag), [athr] "m" (athr)
              : "xmm0",  "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
            );
          }
    r0;
}


  
