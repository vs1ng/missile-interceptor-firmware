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

double normalize(double* A){
    return square_root(mul(*(A+0),*(A+0))+mul(*(A+1),*(A+1))+mul(*(A+2),*(A+2)));
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
    DBL reqSpeed = divide(d,t);
    DBL DIRX = divide(RX,d);
    DBL DIRY = divide(RY,d);
    DBL DIRZ = divide(RZ,d);
    DBL VIX = mul(DIRX,reqSpeed);
    DBL VIY = mul(DIRY,reqSpeed);
    DBL VIZ = mul(DIRZ,reqSpeed);
    DBL AX = divide(VIX,t);
    DBL AY = divide(VIY,t);
    DBL AZ = add(divide(VIZ,t),g);
    DBL PosM[3] = {0,0,0};
    DBL VelM[3] = {0,0,0};
    DBL AccM[3] = {AX,AY,AZ};
    
    DBL PosT[3] = {T.X0,T.Y0,T.Z0};
    DBL VelT[3] = {T.VX0,T.VY0,T.Z0};
    DBL Time = 0;
    while ( d > hit_r ){
        Time+=dt;
        if(Time < burn_time){
            DBL mass = sub(mass_i,mul(fuel_rt,t));
            DBL speed = normalize(&VelM[0]);
            DBL forward[3] = {divide(VIX,speed),divide(VIY,speed),divide(VIZ,speed)};
            DBL TH2M = divide(thrust,mass);
            DBL a_thrust[3] = {forward[0]*TH2M,forward[1]*TH2M,forward[2]*TH2M};
            DBL drag_mag = divide(mul(mul(mul(speed,speed),mul(A,Cd)),mul(0.5,rho)),mass);
            DBL a_drag = -mul(normalize(&VelM[0]),drag_mag);
            DBL a_g[3] = {0,0,-g};
            DBL r[3] = {
                sub(*(PosT+0),*(PosM+0)),
                sub(*(PosT+1),*(PosM+1)),
                sub(*(PosT+2),*(PosM+2))
            };
            DBL R = normalize(&r[0]);
            DBL v[3] = {
                sub(*(VelT+0),*(VelM+0)),
                sub(*(VelT+1),*(VelM+1)),
                sub(*(VelT+2),*(VelM+2))
            };
            DBL Vclose = divide(add(mul(r[0],v[0]),add(mul(r[1],v[1]),mul(r[2],v[2]))),R);
        } else {
            DBL mass = dry_mass;
            DBL a_thrust = 0;
        }
    }
    r0;
}
