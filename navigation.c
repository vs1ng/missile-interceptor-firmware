#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
typedef double DBL;
#define r0 return 0

const DBL g = 9.8;
struct POS1{
    DBL x1, y1, z1;
};

struct POS2{
    DBL x2, y2, z2;
};

struct VEL{
    DBL vx, vy, vz;
};

struct Target{
    struct POS1 P_1;
    struct POS2 P_2;
    struct VEL V;
};

DBL interceptEqn(DBL t, DBL x, DBL y, DBL z, DBL vx, DBL vy, DBL vz, DBL V0) {
    DBL term1 = pow(x + vx*t, 2);
    DBL term2 = pow(y + vy*t, 2);
    DBL term3 = pow(z + vz*t - 0.5*g*t*t, 2);
    DBL term4 = V0*V0 * t*t;
    
    return term1 + term2 + term3 - term4;
}

DBL distance(DBL X1,DBL X2, DBL Y1, DBL Y2){
    return sqrt(
                pow((X1-X2),2)
                 + pow((Y1-Y2),2)
            );
}

int main(int A_C, char* A_G[]){
    srand(time(NULL));
    double t = (int)rand()%(5-2+1)+2;
    if (A_C != 10){
        printf("Needed 7 args, got %i instead. Try again.\n",A_C);
        exit(EXIT_FAILURE);
     }
    struct Target Missle;
    struct Target* MINFO = &Missle;
    MINFO->P_1.x1 = atof(A_G[1]);
    MINFO->P_1.y1 = atof(A_G[2]);
    MINFO->P_1.z1 = atof(A_G[3]);
    MINFO->P_2.x2 = atof(A_G[4]);
    MINFO->P_2.y2 = atof(A_G[5]);
    MINFO->P_2.z2 = atof(A_G[6]);
    MINFO->V.vx = atof(A_G[7]);
    MINFO->V.vy = atof(A_G[8]);
    MINFO->V.vz = atof(A_G[9]);
    printf("Incoming Missle P1:\tIncoming Missle P2:\t\n(%.2f,%.2f,%.2f)\t(%.2f,%.2f,%.2f)\t\n",
            MINFO->P_1.x1,
            MINFO->P_1.y1,
            MINFO->P_1.z1,
            MINFO->P_2.x2,
            MINFO->P_2.y2,
            MINFO->P_2.z2
        );
    DBL targetX = ((MINFO->P_1.x1+MINFO->P_2.x2)/2)+((MINFO->V.vx)*t);
    DBL targetY = ((MINFO->P_1.y1+MINFO->P_2.y2)/2)+((MINFO->V.vy)*t);
    DBL targetZ = ((MINFO->P_1.z1+MINFO->P_2.z2)/2)+((MINFO->V.vz)*t)-((0.5)*(g)*(pow(t,2)));
    DBL avX = (MINFO->P_1.x1+MINFO->P_2.x2)/2;
    DBL axY = (MINFO->P_1.y1+MINFO->P_2.y2)/2;
    DBL axZ = (MINFO->P_1.z1+MINFO->P_2.z2)/2;
    for(int T = (int)ceil(t); T < 40 ; T++){
        DBL value = interceptEqn(T,avX, axY, axZ, MINFO->V.vx, MINFO->V.vy, MINFO->V.vz, sqrt(pow(MINFO->V.vx,2)+pow(MINFO->V.vy,2)+pow(MINFO->V.vz,2)));
        printf("T=%i\tValue=%f\n",T,value);
        if(value > 0){
            puts("FOUND +VE!!");
            for(DBL t_2 = T-1;t_2 < T+1;t_2+=0.1){
                DBL value_2 = interceptEqn(t_2,avX, axY, axZ, MINFO->V.vx, MINFO->V.vy, MINFO->V.vz, sqrt(pow(MINFO->V.vx,2)+pow(MINFO->V.vy,2)+pow(MINFO->V.vz,2)));
                printf("t_2=%f\tValue=%f\n",t_2,value_2);
            }
            exit(EXIT_SUCCESS);
        }
    }     
    r0;
}
