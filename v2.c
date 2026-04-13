#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#define r0 return 0
typedef double DBL;
typedef const double CD;

CD g = 9.81;
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

void normalize(double *out, double *A) {
    double mag = square_root(mul(A[0], A[0]) + mul(A[1], A[1]) + mul(A[2], A[2]));
    if (mag == 0) {
        out[0] = out[1] = out[2] = 0;
    } else {
        out[0] = divide(A[0], mag);
        out[1] = divide(A[1], mag);
        out[2] = divide(A[2], mag);
    }
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
    DBL Time = 0;
    DBL PosT[3] = {T.X0, T.Y0, T.Z0};
    DBL VelT[3] = {T.VX0, T.VY0, T.VZ0};

    DBL Rel[3] = {sub(PosT[0], PosM[0]), sub(PosT[1], PosM[1]), sub(PosT[2], PosM[2])};
    DBL d = square_root(mul(Rel[0], Rel[0]) + mul(Rel[1], Rel[1]) + mul(Rel[2], Rel[2]));
    DBL reqSpeed = divide(d,t);

    while (d > hit_r) {
        Time = add(Time, dt);
        DBL mass;
        if (Time < burn_time) {
            mass = sub(mass_i, mul(fuel_rt, Time));
        } else {
            mass = dry_mass;
        }
        DBL speed = square_root(mul(VelM[0], VelM[0]) + mul(VelM[1], VelM[1]) + mul(VelM[2], VelM[2]));
        DBL forward[3] = {0,0,0};
        if (speed > 0.001) {
            forward[0] = divide(VelM[0], speed);
            forward[1] = divide(VelM[1], speed);
            forward[2] = divide(VelM[2], speed);
        } else {
            forward[0] = divide(DIRX, 1.0);
            forward[1] = divide(DIRY, 1.0);
            forward[2] = divide(DIRZ, 1.0);
        }
        DBL a_thrust[3] = {0,0,0};
        if (Time < burn_time) {VelT
            DBL thrust_acc_mag = divide(thrust, mass);VelT
            a_thrust[0] = mul(forward[0], thrust_acc_mag);
            a_thrust[1] = mul(forward[1], thrust_acc_mag);
            a_thrust[2] = mul(forward[2], thrust_acc_mag);
        }
        DBL drag_mag = divide(mul(mul(mul(speed, speed), mul(A, Cd)), mul(0.5, rho)), mass);
        DBL a_drag[3];
        if (speed > 0.001) {
            a_drag[0] = mul(-forward[0], drag_mag);
            a_drag[1] = mul(-forward[1], drag_mag);
            a_drag[2] = mul(-forward[2], drag_mag);
        } else {
            a_drag[0] = a_drag[1] = a_drag[2] = 0;
        }

        DBL a_grav[3] = {0, 0, -g};
        DBL r[3] = {sub(PosT[0], PosM[0]), sub(PosT[1], PosM[1]), sub(PosT[2], PosM[2])};
        DBL R = square_root(mul(r[0], r[0]) + mul(r[1], r[1]) + mul(r[2], r[2]));
        DBL v[3] = {sub(VelT[0], VelM[0]), sub(VelT[1], VelM[1]), sub(VelT[2], VelM[2])};
        DBL Vc = -divide(add(mul(r[0], v[0]), add(mul(r[1], v[1]), mul(r[2], v[2]))), R);
        DBL cross[3];
        cross[0] = sub(mul(r[1], v[2]), mul(r[2], v[1]));
        cross[1] = sub(mul(r[2], v[0]), mul(r[0], v[2]));
        cross[2] = sub(mul(r[0], v[1]), mul(r[1], v[0]));
        DBL cross_mag = square_root(mul(cross[0], cross[0]) + mul(cross[1], cross[1]) + mul(cross[2], cross[2]));
        DBL lambda_dot = divide(cross_mag, mul(R, R));

        DBL a_cmd = mul(mul(N, Vc), lambda_dot);

        DBL perp_dir[3];
        perp_dir[0] = sub(mul(cross[1], r[2]), mul(cross[2], r[1]));
        perp_dir[1] = sub(mul(cross[2], r[0]), mul(cross[0], r[2]));
        perp_dir[2] = sub(mul(cross[0], r[1]), mul(cross[1], r[0]));
        DBL perp_mag = square_root(mul(perp_dir[0], perp_dir[0]) + mul(perp_dir[1], perp_dir[1]) + mul(perp_dir[2], perp_dir[2]));
        if (perp_mag > 0.001) {
            perp_dir[0] = divide(perp_dir[0], perp_mag);
            perp_dir[1] = divide(perp_dir[1], perp_mag);
            perp_dir[2] = divide(perp_dir[2], perp_mag);
        } else {
            perp_dir[0] = perp_dir[1] = perp_dir[2] = 0;
        }

        DBL a_guidance[3] = {mul(a_cmd, perp_dir[0]), mul(a_cmd, perp_dir[1]), mul(a_cmd, perp_dir[2])};

        if (R < 300) {
            DBL t_go = divide(R, Vc);
            if (t_go < 0.01) t_go = 0.01;
            DBL ZEM[3] = {
                add(r[0], mul(v[0], t_go)),
                add(r[1], mul(v[1], t_go)),
                add(r[2], mul(v[2], t_go))
            };
            DBL a_term_mag = divide(2.0, mul(t_go, t_go));
            a_guidance[0] = mul(a_term_mag, ZEM[0]);
            a_guidance[1] = mul(a_term_mag, ZEM[1]);
            a_guidance[2] = mul(a_term_mag, ZEM[2]);
        }

        DBL a_total[3] = {
            add(add(add(a_guidance[0], a_thrust[0]), a_drag[0]), a_grav[0]),
            add(add(add(a_guidance[1], a_thrust[1]), a_drag[1]), a_grav[1]),
            add(add(add(a_guidance[2], a_thrust[2]), a_drag[2]), a_grav[2])
        };

        if (speed > 0.001) {
            DBL a_forward_mag = add(mul(a_total[0], forward[0]), add(mul(a_total[1], forward[1]), mul(a_total[2], forward[2])));
            DBL a_forward[3] = {mul(a_forward_mag, forward[0]), mul(a_forward_mag, forward[1]), mul(a_forward_mag, forward[2])};
            DBL a_lateral[3] = {sub(a_total[0], a_forward[0]), sub(a_total[1], a_forward[1]), sub(a_total[2], a_forward[2])};
            DBL lat_mag = square_root(mul(a_lateral[0], a_lateral[0]) + mul(a_lateral[1], a_lateral[1]) + mul(a_lateral[2], a_lateral[2]));
            if (lat_mag > max_turn) {
                DBL scale = divide(max_turn, lat_mag);
                a_lateral[0] = mul(a_lateral[0], scale);
                a_lateral[1] = mul(a_lateral[1], scale);
                a_lateral[2] = mul(a_lateral[2], scale);
            }
            a_total[0] = add(a_forward[0], a_lateral[0]);
            a_total[1] = add(a_forward[1], a_lateral[1]);
            a_total[2] = add(a_forward[2], a_lateral[2]);
        }

        VelM[0] = add(VelM[0], mul(a_total[0], dt));
        VelM[1] = add(VelM[1], mul(a_total[1], dt));
        VelM[2] = add(VelM[2], mul(a_total[2], dt));

        PosM[0] = add(PosM[0], mul(VelM[0], dt));
        PosM[1] = add(PosM[1], mul(VelM[1], dt));
        PosM[2] = add(PosM[2], mul(VelM[2], dt));

        PosT[0] = add(PosT[0], mul(VelT[0], dt));
        PosT[1] = add(PosT[1], mul(VelT[1], dt));
        PosT[2] = add(PosT[2], mul(VelT[2], dt));

        VelT[2] = sub(VelT[2], mul(g, dt));
        Rel[0] = sub(PosT[0], PosM[0]);
        Rel[1] = sub(PosT[1], PosM[1]);
        Rel[2] = sub(PosT[2], PosM[2]);
        d = square_root(mul(Rel[0], Rel[0]) + mul(Rel[1], Rel[1]) + mul(Rel[2], Rel[2]));
    }

    printf("Hit at time = %.2f seconds\n", Time);
    r0;
}
