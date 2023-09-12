#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415927
#endif

/*
given information
- l0 = 0.25, l1 = 0.2, l2 = 0.2, l3 = 0.15
- d1 = -0.04, d2 = 0.04, d3 = -0.04, d3 = -0.04 (d1 is between joint 1 and 2, etc)

*/

int R = 0; //rotational
int L = 1; //translational by the link
int D = 2; //translational by the offset

int x_axis = 0;
int y_axis = 1;
int z_axis = 2;

// length of joints
double l[4] = {0.25, 0.2, 0.2, 0.15};

// 1-idx array to hold the offset
// index 0 is skiped to align with the problem's description
double d[5] = {0, -0.04, 0.04, -0.04, -0.04};

// for debugging
void print_matrix(double A[4][4]){
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%.4lf\t", A[i][j]); // You can adjust the format specifier as needed
        }
        printf("\n"); // Move to the next row after printing each row
    }
}

// this functin take in an angle (in radian), what axis the frame rotate about
// and output the rotational matrix
// utilize homogeneous transforms -> the matrixis 4x4
// Chat-GPT was used to generate part of the code
void rotational_matrix(double angle, int axis, double matrix[4][4]){
    double c = cos(angle);
    double s = sin(angle);

    // Initialize the matrix as an identity matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j) {
                matrix[i][j] = 1.0;
            } else {
                matrix[i][j] = 0.0;
            }
        }
    }
    
    switch (axis) {
        case 0:  // Rotate about the X-axis
            matrix[1][1] = c;
            matrix[1][2] = -s;
            matrix[2][1] = s;
            matrix[2][2] = c;
            break;
        case 1:  // Rotate about the Y-axis
            matrix[0][0] = c;
            matrix[0][2] = s;
            matrix[2][0] = -s;
            matrix[2][2] = c;
            break;
        case 2:  // Rotate about the Z-axis
            matrix[0][0] = c;
            matrix[0][1] = -s;
            matrix[1][0] = s;
            matrix[1][1] = c;
            break;
        default:
            printf("Invalid axis. Please use 0 (X-axis), 1 (Y-axis), or 2 (Z-axis).\n");
    }
}

void translational_matrix(double length, int axis, double matrix[4][4]){
    //printf("%f\n", length);
    // Initialize the matrix as an identity matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j) {
                matrix[i][j] = 1.0;
            } else {
                matrix[i][j] = 0.0;
            }
        }
    }

    // set the P vector, it's located at the last col
    matrix[axis][3] = length;
    //print_matrix(matrix);
}

// this function multiply two (4x4) matrices
// utilized chat-GPT for saving time purpose
void mul_2_matrices(double A[4][4], double B[4][4], double res[4][4]){
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            res[i][j] = 0.0;
            for (int k = 0; k < 4; k++) {
                res[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// based on the descripton of the robot
// transformation function: 
// Rz(theta0)Dz(l0)Ry(theta1)Dy(d1)Dx(l1)Ry(theta2)Dy(d2)Dx(l2)Ry(theta3)Dz(d4)Rx(theta4)Dx(l3)
fwd_kin(theta, x)
// this is a pointer to an array of 6 angles (we only need to care the first 5)
// the angle is in RADIAN
double *theta; 
// the function should output vector position, given the set of angles
double x[3]; 
{   
    // type of operations
    int operation[] = {R, L, R, D, L, R, D, L, R, D, R, L};
    int axis[] = {z_axis, z_axis, y_axis, y_axis, x_axis, y_axis, y_axis, x_axis, y_axis, z_axis, x_axis, x_axis};
    
    double T[4][4];
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            if (i==j){
                T[i][j] = 1.0;
            }
            else{
                T[i][j] = 0.0;
            }  
        }
    }
    
    int p_theta = 4;
    int p_link = 3;
    int p_offset = 4;

    for (int p = 11; p>=0; p--){
        double temp[4][4];
        if(operation[p] == R){
            rotational_matrix(theta[p_theta], axis[p], temp);
            p_theta -=1;
        }
        else if (operation[p] == L){
            translational_matrix(l[p_link], axis[p], temp);
            p_link -=1;
        }
        else{
            translational_matrix(d[p_offset], axis[p], temp);
            p_offset -=1;
        }
        //print_matrix(temp);
        double res[4][4];
        mul_2_matrices(T, temp, res);
        for (int i=0; i<4; i++){
            for (int j=0; j<4; j++){
                T[i][j] = res[i][j];
            }
        }
        print_matrix(T);
    }

    // assign x
    x[0] = T[0][3];
    x[1] = T[1][3];
    x[2] = T[2][3];

}


inv_kin(x, theta)
double *x;
double theta[6];
{
}








