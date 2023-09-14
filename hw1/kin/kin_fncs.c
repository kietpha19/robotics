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

const int R = 0; //rotational
const int L = 1; //translational by the link
const int D = 2; //translational by the offset

const int x_axis = 0;
const int y_axis = 1;
const int z_axis = 2;

// length of joints
const double l[4] = {0.25, 0.2, 0.2, 0.15};

// 1-idx array to hold the offset
// index 0 is skiped to align with the problem's description
const double d[5] = {0, -0.04, 0.04, -0.04, -0.04};

// for debugging
void print_matrix(double A[4][4]){
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%.4lf\t", A[i][j]); // You can adjust the format specifier as needed
        }
        printf("\n"); // Move to the next row after printing each row
    }
    printf("\n");
}

// this functin take in an angle (in radian), what axis the frame rotate about
// and output the rotational matrix
// utilize homogeneous transforms -> the matrix is 4x4
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
// Rz(theta0)Dz(l0)Ry(theta1)Dy(d1)Dx(l1)Ry(theta2)Dy(d2)Dx(l2)Ry(theta3)Dy(d3)Rx(theta4)Dz(d4)Dx(l3)
fwd_kin(theta, x)
// this is a pointer to an array of 6 angles (we only need to care the first 5)
// the angle is in RADIAN
double *theta; 
// the function should output vector position, given the set of angles
double x[3]; 
{   
    // type of operations
    int operation[] = {R, L, R, D, L, R, D, L, R, D, D, R, L};
    int axis[] = {z_axis, z_axis, y_axis, y_axis, x_axis, y_axis, y_axis, x_axis, y_axis, y_axis, z_axis, x_axis, x_axis};
    
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

    for (int p = 12; p>=0; p--){
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
        mul_2_matrices(temp, T, res); // IMPORTANT: the order does matter;

        // copy res to T
        for (int i=0; i<4; i++){
            for (int j=0; j<4; j++){
                T[i][j] = res[i][j];
            }
        }
        //print_matrix(T);
    }
    //printf("%d", p_offset);

    // assign x, y, z
    x[0] = T[0][3];
    x[1] = T[1][3];
    x[2] = T[2][3];
}

//link lengths
const double l0 = 0.25;
const double l1 = 0.2;
const double l2 = 0.2;
const double l3 = 0.15;
//offsets
const double d1 = -0.04;
const double d2 = 0.04;
const double d3 = -0.04;
const double d4 = -0.04;

// for this function, based on the required orientation of the tool frame
// theta4 = 0
// phi = pi/2
inv_kin(x, theta)
double *x;
double theta[6];
{
    double alpha;
    double beta;
    double gamma;
    double nom; //nominator
    double den; //denominator

    double px = x[0];
    double py = x[1];
    double pz = x[2];

    theta[5] = 0.0;
    theta[4] = 0.0;
    double phi = M_PI/2.0;

    // calculate theta[0]
    alpha = atan2(py, px);
    double r = sqrt(px*px + py*py);
    beta = atan2(d1, sqrt(r*r - d[1]*d[1]));
    
    theta[0] =  alpha - beta;


    // calcualte the wrist position
    px = px + d[4]*sin(theta[0]) - d[3]*cos(theta[0]);
    py = py - d[4]*cos(theta[0]) - d[3]*sin(theta[0]); 
    pz = pz + l[3];

    // move base frame to join 1
    px = sqrt(px*px + py*py);
    py = 0;
    pz = pz - l[0];

    // calculate theta2 using the fomular derived in class
    // notice this is now in x-z plane
    nom = px*px + pz*pz - l[1]*l[1] - l[2]*l[2];
    den = 2*l1*l2;
    theta[2] = acos(nom/den);

    //calculate theta1
    gamma = atan2(pz,px);
    nom = l[1]*l[1] + px*px + pz*pz -l[2]*l[2];
    den = 2*l[1]*sqrt(px*px + pz*pz);
    alpha = acos(nom/den);
    if(theta[2] > 0){
        theta[1] = gamma + alpha;
    }
    else{
        theta[1] = gamma - alpha;
    }

    theta[1] *= (-1); /// WHYYYYY is this? I don't know
    // I see why now, because positive y-axis is pointing inward the screen
    // unlike the lecture slide, the positive z-axis is pointing outward
    // WHY MAKE THIS ASSIGNMENT SO COMPLICATED
    // TOOK ME 4-HOURS SLEEP ONLY DAYSSSSSS

    //calcualte theta3
    theta[3] = phi - theta[1] - theta[2];
}








