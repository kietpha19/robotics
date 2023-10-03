// Include neccessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Define constants
#ifndef M_PI
#define M_PI 3.1415927
#endif

#ifndef MATRIX_DIM
#define MATRIX_DIM 4
#endif


// Function initialize origin coordinates in form of homogeneous
double** init_homogeneous_origin(int rows, int cols) {
    
    double** origin = (double**)malloc(rows * sizeof(double*));
    
    if (origin == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        origin[i] = (double*)malloc(cols * sizeof(double));
        if (origin[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (i < rows - 1) {
                origin[i][j] = 0;
            } else origin[i][j] = 1;
        }
    }

    return origin;
}


// Function to create and return a dynamically allocated 2D matrix
double** T_mat(double rot_angle, int dim, double trans_x, double trans_y, double trans_z, char rot_axis) {
    
    double** mat = (double**)malloc(dim * sizeof(double*));

    if (mat == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < dim; i++) {
        mat[i] = (double*)malloc(dim * sizeof(double));
        if (mat[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }

    // Fill in matrix based on its rotation matrix
    if (rot_axis == 'x') {
        mat[0][0] = 1, mat[0][1] = 0,              mat[0][2] = 0,               mat[0][3] = trans_x;
        mat[1][0] = 0, mat[1][1] = cos(rot_angle), mat[1][2] = -sin(rot_angle), mat[1][3] = trans_y;
        mat[2][0] = 0, mat[2][1] = sin(rot_angle), mat[2][2] = cos(rot_angle),  mat[2][3] = trans_z;
        mat[3][0] = 0, mat[3][1] = 0,              mat[3][2] = 0,               mat[3][3] = 1;

    } else if (rot_axis == 'y') {
        mat[0][0] = cos(rot_angle),  mat[0][1] = 0, mat[0][2] = sin(rot_angle), mat[0][3] = trans_x;
        mat[1][0] = 0,               mat[1][1] = 1, mat[1][2] = 0,              mat[1][3] = trans_y;
        mat[2][0] = -sin(rot_angle), mat[2][1] = 0, mat[2][2] = cos(rot_angle), mat[2][3] = trans_z;
        mat[3][0] = 0,               mat[3][1] = 0, mat[3][2] = 0,               mat[3][3] = 1;

    } else if (rot_axis == 'z') {
        mat[0][0] = cos(rot_angle), mat[0][1] = -sin(rot_angle), mat[0][2] = 0, mat[0][3] = trans_x;
        mat[1][0] = sin(rot_angle), mat[1][1] = cos(rot_angle),  mat[1][2] = 0, mat[1][3] = trans_y;
        mat[2][0] = 0,              mat[2][1] = 0,               mat[2][2] = 1, mat[2][3] = trans_z;
        mat[3][0] = 0,              mat[3][1] = 0,               mat[3][2] = 0, mat[3][3] = 1;

    } else {
        printf("Invalid choice!\n");
    }

    return mat;
}


// Function to perform matrix multiplication
double** multiply(double** mat1, double** mat2, int dim, char choice) {
    
    int res_dim;
    if (choice == 'm') {
        res_dim = 4;
    } else if (choice == 'p') {
        res_dim = 1;
    }

    double** res_mat = (double**)malloc(dim * sizeof(double*));
    for (int i = 0; i < dim; i++) {
        res_mat[i] = (double*)malloc(res_dim * sizeof(double));
    }

    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < res_dim; j++) {
            res_mat[i][j] = 0;
            for (int k = 0; k < dim; k++) {
                res_mat[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }

    return res_mat;
}


// Function to print matrix
void print_mat(double** mat, int dim, char choice) {

    int cols;
    if (choice == 'm') {
        cols = 4;
    } else if (choice == 'p') {
        cols = 1;
    }

    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.2f ", mat[i][j]);
        }
        printf("\n");
    }
}


// Function to free a dynamically allocated matrix
void free_mat(double** mat, int dim) {
    for (int i = 0; i < dim; i++) {
        free(mat[i]);
    }
    free(mat);
}


// Function of forward kinematics 
fwd_kin(theta, x)
double *theta;
double x[3];
{   

    // Robot frame lengths
    float l_0 = 0.25, l_1 = 0.20, l_2 = 0.20, l_3 = 0.15;
    float d_1 = -0.04, d_2 = 0.04, d_3 = -0.04, d_4 = -0.04; 

    double** org_coords = init_homogeneous_origin(MATRIX_DIM, 1);
    double** trans_mat_B1 = T_mat(theta[0], MATRIX_DIM, 0, 0, l_0, 'z');
    double** trans_mat_12 = T_mat(theta[1], MATRIX_DIM, l_1, d_1, 0, 'y');
    double** trans_mat_23 = T_mat(theta[2], MATRIX_DIM, l_2, d_2, 0, 'y');
    double** trans_mat_3T = T_mat(theta[3], MATRIX_DIM, l_3, d_3, d_4, 'y');

    double** end_coords1 = multiply(trans_mat_B1, org_coords, MATRIX_DIM, 'p');
    double** end_coords2 = multiply(trans_mat_12, end_coords1, MATRIX_DIM, 'p');
    double** end_coords3 = multiply(trans_mat_23, end_coords2, MATRIX_DIM, 'p');
    double** end_coords = multiply(trans_mat_3T, end_coords3, MATRIX_DIM, 'p');

    x[0] = end_coords[0][0];
    x[1] = end_coords[1][0];
    x[2] = end_coords[2][0];

}


inv_kin(x, theta)
double *x;
double theta[6];
{

    // Robot frame lengths
    float l_0 = 0.25, l_1 = 0.20, l_2 = 0.20, l_3 = 0.15;
    float d_1 = -0.04, d_2 = 0.04, d_3 = -0.04, d_4 = -0.04; 

    // Removing frame (34) and re-calculate x_prime, y_prime, z_prime
    float x_prime = x[0] - l_3, y_prime = x[1] - d_3, z_prime = x[2] - d_4;

    // Compute theta 0
    theta[0] = atan2(y_prime, x_prime);

    // Romving framme (01) and re-calculate x_prime_prime, y_prime_prime, z_prime_prime  
    float x_prime_prime = sqrt(pow(x_prime, 2) + pow(y_prime, 2)), y_prime_prime = 0, z_prime_prime = z_prime - l_0;

    // Compute theta 2
    theta[2] = acos((pow(x_prime_prime, 2) + pow(y_prime_prime, 2) - pow(l_1, 2) - pow(l_2, 2))/2*l_1*l_2);

    // Compute alpha and gamma
    double alpha = acos((pow(l_1, 2) - pow(l_2, 2) + pow(x_prime_prime, 2) + pow(y_prime_prime, 2))/(2*l_1*sqrt(pow(x_prime_prime, 2) + pow(y_prime_prime, 2))));
    double gamma = atan2(y_prime_prime, x_prime_prime);

    // Compute theta 1 based on sign on theta 2
    if (theta[2] >= 0) {
        theta[1] = gamma - alpha;
    } else {
        theta[1] = gamma + alpha;
    }

    // Compute theta 3
    //double theta;
    theta[3] = atan2(0, 1) - theta[1] - theta[2]; 
    
    // Theta 4 does not matter so set it as 0
    theta[4] = 0;

}
