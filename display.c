#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define r0 return 0
typedef double DBL;
typedef const double CD;

CD g = 9.81;
CD dt = 0.01;
CD N = 4;
CD burn_time = 20.0;
CD mass_i = 80;
CD fuel_mass = 20;
CD thrust = 15000;
CD rho = 1.225;
CD Cd = 0.5;
CD A = 0.02;
const char hit_r = 3;
DBL max_turn = (30.0)*(9.81); 
DBL fuel_rt = 20.0/3.0;
DBL dry_mass = 80.0-20.0;

double add(double A, double B){
    DBL res;
    asm volatile("movsd %[A], %%xmm0\n\t" "movsd %[B], %%xmm1\n\t"
                 "addsd %%xmm1, %%xmm0\n\t" "movsd %%xmm0, %[res]\n\t"
                 : [res] "=m" (res) : [A] "m" (A), [B] "m" (B) : "xmm0", "xmm1");
    return res;
}
double sub(double A, double B){
    DBL res;
    asm volatile("movsd %[A], %%xmm0\n\t" "movsd %[B], %%xmm1\n\t"
                 "subsd %%xmm1, %%xmm0\n\t" "movsd %%xmm0, %[res]\n\t"
                 : [res] "=m" (res) : [A] "m" (A), [B] "m" (B) : "xmm0", "xmm1");
    return res;
}
double mul(double A, double B){
    DBL res;
    asm volatile("movsd %[A], %%xmm0\n\t" "movsd %[B], %%xmm1\n\t"
                 "mulsd %%xmm1, %%xmm0\n\t" "movsd %%xmm0, %[res]\n\t"
                 : [res] "=m" (res) : [A] "m" (A), [B] "m" (B) : "xmm0", "xmm1");
    return res;
}
double divide(double A, double B){
    double res;
    asm volatile("movsd %[A], %%xmm0\n\t" "movsd %[B], %%xmm1\n\t"
                 "divsd %%xmm1, %%xmm0\n\t" "movsd %%xmm0, %[res]\n\t"
                 : [res] "=m" (res) : [A] "m" (A), [B] "m" (B) : "xmm0", "xmm1");
    return res;
}
double square_root(double A) {
    double res;
    asm volatile("sqrtsd %[A], %%xmm0\n\t" "movsd %%xmm0, %[res]\n\t"
                 : [res] "=m" (res) : [A] "m" (A) : "xmm0");
    return res;
}

struct Threat{ DBL X0, Y0, Z0; DBL VX0, VY0, VZ0; };
void problem(int sig){ printf("\033[0m\n"); exit(0); }

int main(int ARGC, char* ARG[]){
    signal(SIGINT, problem);
    if (ARGC != 7) return 1;

    struct Threat T;
    T.VX0 = atof(ARG[1]); T.VY0 = atof(ARG[2]); T.VZ0 = atof(ARG[3]);
    T.X0 = atof(ARG[4]); T.Y0 = atof(ARG[5]); T.Z0 = atof(ARG[6]);

    DBL Time = 0;
    DBL PosM[3] = {0, 0, 0}; 
    DBL VelM[3] = {0, 0, 0};
    DBL PosT[3] = {T.X0, T.Y0, T.Z0};
    DBL VelT[3] = {T.VX0, T.VY0, T.VZ0};

    DBL t_p = 2.0;
    DBL xt = add(T.X0, mul(T.VX0, t_p));
    DBL zt = sub(add(T.Z0, mul(T.VZ0, t_p)), mul(4.9, mul(t_p, t_p)));
    DBL d = square_root(add(mul(xt, xt), mul(zt, zt)));
    
    VelM[0] = 150.0; 
    VelM[2] = 150.0; 

    while (d > hit_r && Time < 60.0 && PosM[2] >= -1.0) {
        Time = add(Time, dt);
        DBL mass = (Time < burn_time) ? sub(mass_i, mul(fuel_rt, Time)) : dry_mass;
        DBL speed = square_root(add(mul(VelM[0], VelM[0]), mul(VelM[2], VelM[2])));
        
        DBL fwd[3] = { divide(VelM[0], speed), 0, divide(VelM[2], speed) };
        DBL a_tot[3] = {0, 0, -g};

        if (Time < burn_time) {
            DBL acc = divide(thrust, mass);
            a_tot[0] = add(a_tot[0], mul(fwd[0], acc));
            a_tot[2] = add(a_tot[2], mul(fwd[2], acc));
        }

        DBL r[3] = { sub(PosT[0], PosM[0]), 0, sub(PosT[2], PosM[2]) };
        DBL v[3] = { sub(VelT[0], VelM[0]), 0, sub(VelT[2], VelM[2]) };
        DBL R_mag = square_root(add(mul(r[0], r[0]), mul(r[2], r[2])));
        DBL Vc = -divide(add(mul(r[0], v[0]), mul(r[2], v[2])), R_mag);

        const char* color = "\033[1;34m";
        const char* status = "LAUNCH";

        if (R_mag <= hit_r) {
            color = "\033[1;32m";
            status = "HIT!";
        } else if (R_mag < 300) {
            color = "\033[1;31m";
            status = "ZEM";
            DBL t_go = divide(R_mag, Vc);
            if (t_go < 0.01) t_go = 0.01;
            DBL a_zem[3] = {
                mul(divide(2.0, mul(t_go, t_go)), add(r[0], mul(v[0], t_go))),
                0,
                mul(divide(2.0, mul(t_go, t_go)), add(r[2], mul(v[2], t_go)))
            };
            a_tot[0] = add(a_tot[0], a_zem[0]);
            a_tot[2] = add(a_tot[2], a_zem[2]);
        } else {
            if (R_mag < 600) { color = "\033[38;5;208m"; status = "NEARING"; }
            else if (Time < burn_time) { color = "\033[1;33m"; status = "BOOST"; }
            
            DBL cross_y = sub(mul(r[2], v[0]), mul(r[0], v[2]));
            DBL lambda_dot = divide(cross_y, mul(R_mag, R_mag));
            DBL a_pn_mag = mul(mul(N, Vc), lambda_dot);
            a_tot[0] = add(a_tot[0], mul(a_pn_mag, -fwd[2]));
            a_tot[2] = add(a_tot[2], mul(a_pn_mag, fwd[0]));
        }

        for(int i=0; i<3; i++) {
            VelM[i] = add(VelM[i], mul(a_tot[i], dt));
            PosM[i] = add(PosM[i], mul(VelM[i], dt));
            PosT[i] = add(PosT[i], mul(VelT[i], dt));
        }
        VelT[2] = sub(VelT[2], mul(g, dt));
        d = R_mag;

        printf("%sSTATUS: %-10s TIME: %6.2fs DIST: %8.2fm ALT: %7.1fm VC: %7.1fm/s\033[0m\n", 
               color, status, Time, d, PosM[2], Vc);
        usleep(100); 
    }

    r0;
}
