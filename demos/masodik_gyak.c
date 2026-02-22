#include <stdio.h>
#include <math.h>

void transformPoint(double p[2], double M[3][3], double result[2]);
void scale(double M[3][3], double n);
void shift(double M[3][3], double x, double y);
void rotate(double M[3][3], double t);
void writeM(double M[3][3]);

int main() {

    double matrix[3][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };

    double point[2] = {-5, 2};
    double result[2];

    writeM(matrix);

    shift(matrix, -3, 0);
    writeM(matrix);

    rotate(matrix, 360);
    writeM(matrix);

    transformPoint(point, matrix, result);
    
    for (int i = 0; i < 2; i++) {
        printf("%d ", result[i]);
    }       

}

void transformPoint(double p[2], double M[3][3], double result[2]) {

    result[0] = M[0][0]*p[0]+M[0][1]*p[1]+M[0][2]*1;
    result[1] = M[1][0]*p[0]+M[1][1]*p[1]+M[1][2]*1;
}

void scale(double M[3][3], double n) {

    M[0][0] *= n;
    M[1][1] *= n;

}

void shift(double M[3][3], double x, double y) {
    
    M[0][2] += x;
    M[1][2] += y;

}

void rotate(double M[3][3], double t) {
    
    double cosT = cos(t * M_PI / 180.0);
    double sinT = sin(t * M_PI / 180.0);

    M[0][0] = cosT;
    M[0][1] = -sinT;

    M[1][0] = sinT;
    M[1][1] = cosT;

}

void writeM(double M[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%.2f ", M[i][j]);
        } 
        printf("\n");
    } 
    printf("\n");
}