#include <stdio.h>
#include <math.h>
#include <stdlib.h>
typedef double DBL;
#define r0 return 0

struct POS1{
    DBL x1, y1, z1;
};

struct POS2{
    DBL x2, y2, z2;
};

struct Target{
    struct POS1 P_1;
    struct POS2 P_2;
    double V;
};


DBL distance(DBL X1,DBL X2, DBL Y1, DBL Y2){
    return sqrt(
                pow((X1-X2),2)
                 + pow((Y1-Y2),2)
            );
}

int main(int A_C, char* A_G[]){
    if (A_C != 8){
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
    MINFO->V=atof(A_G[7]);
    printf("Incoming Missle P1:\tIncoming Missle P2:\tVelocity:\n(%.2f,%.2f,%.2f)\t(%.2f,%.2f,%.2f)\t%.2f\n",
            MINFO->P_1.x1,
            MINFO->P_1.y1,
            MINFO->P_1.z1,
            MINFO->P_2.x2,
            MINFO->P_2.y2,
            MINFO->P_2.z2,
            MINFO->V);
    r0;
}
