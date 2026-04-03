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

double add(double A, double B){
    DBL res;
    asm volatile(
            "movsd %[A], %%xmm0\n\t"
            "movsd %[B], %%xmm1\n\t"
            "addsd %%xmm1, %%xmm0\n\t"
            "movsd %%xmm0, %[res]\n\t"
            : [res] "=m" (res)
            : [A] "m" (A), [B] "m" (B)
            : "xmm0", "xmm1"
    );
    return res;
}

double sub(double A, double B){
    DBL res;
    asm volatile(
            "movsd %[A], %%xmm0\n\t"
            "movsd %[B], %%xmm1\n\t"
            "subsd %%xmm1, %%xmm0\n\t"
            "movsd %%xmm0, %[res]\n\t"
            : [res] "=m" (res)
            : [A] "m" (A), [B] "m" (B)
            : "xmm0", "xmm1"
    );
    return res;
}

double mul(double A, double B){
    DBL res;
    asm volatile(
            "movsd %[A], %%xmm0\n\t"
            "movsd %[B], %%xmm1\n\t"
            "mulsd %%xmm1, %%xmm0\n\t"
            "movsd %%xmm0, %[res]\n\t"
            : [res] "=m" (res)
            : [A] "m" (A), [B] "m" (B)
            : "xmm0", "xmm1"
    );
    return res;
}

double divide(double A, double B){
    double res;
    asm volatile(
            "movsd %[A], %%xmm0\n\t"
            "movsd %[B], %%xmm1\n\t"
            "divsd %%xmm1, %%xmm0\n\t"
            "movsd %%xmm0, %[res]\n\t"
            : [res] "=m" (res)
            : [A] "m" (A), [B] "m" (B)
            : "xmm0", "xmm1"
     );
    return res;
}

double square_root(double A) {
    double res;
    asm volatile(
            "sqrtsd %[A], %%xmm0\n\t"
            "movsd %%xmm0, %[res]\n\t"
            : [res] "=m" (res)
            : [A] "m" (A)
            : "xmm0"
    );
    return res;
}

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
    }
    T.VX0 = atof(ARG[1]);
    T.VY0 = atof(ARG[2]);
    T.VZ0 = atof(ARG[3]);
    T.X0 = atof(ARG[4]);
    T.Y0 = atof(ARG[5]);
    T.Z0 = atof(ARG[6]);
    DBL t = 2;

    DBL xt = add(T.X0,mul(T.VX0,t));
    DBL yt = add(T.Y0,mul(T.VY0,t));
    DBL zt = sub(add(T.Z0,mul(T.VZ0,t)),mul(mul(0.5,g),mul(t,t)));
    DBL RX = xt;
    DBL RY = yt;
    DBL RZ = zt;
    DBL d = square_root(mul(RX,RX)+mul(RY,RY)+mul(RZ,RZ));
    DBL reqSpeed = d/t;
    DBL DIRX = RX/d;
    DBL DIRY = RY/d;
    DBL DIRZ = RZ/d;

    r0;
}
