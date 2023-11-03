#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <limits.h>

#define DIM 512

/******************************************************************/
/* This structure contains the coordinates of a box drawn with    */
/* the left mouse button on the image window.                     */
/* roi.x , roi.y  - left upper corner's coordinates               */
/* roi.width , roi.height - width and height of the box           */
/******************************************************************/
extern XRectangle roi;

// for debugging purpose
void print_img(unsigned char image[DIM][DIM], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%4d", (int)image[i][j]); // Cast each element to int before printing
        }
        printf("\n");
    }
}

void zeros_padding(unsigned char image[DIM][DIM], int rows, int cols, int padded_img[DIM][DIM]){
    for(int i=0; i<=rows; i++){
        for(int j=0; j<=cols; j++){
            if(i==0 || i==rows || j==0 || j==cols){
                padded_img[i][j] = 0;
            } else{
                padded_img[i][j] = image[i-1][j-1];
            }
        }
    }
}

void normalize(int output[DIM][DIM], int rows, int cols, int min_value, int max_value, unsigned char proc_img[DIM][DIM]){
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            proc_img[i][j] = (unsigned char)((output[i][j] - min_value)*255.0/(max_value-min_value));
        }
    }
}

void edge_detection(unsigned char image[DIM][DIM], int rows, int cols, unsigned char proc_img[DIM][DIM], int type){
    int padded_img[DIM][DIM];
    int output[DIM][DIM];
    zeros_padding(image, rows, cols, padded_img);
    int kernel[3][3];

    // Define the kernel based on the selected type
    // vertical
    if (type == 1) {
        kernel[0][0] = -1;
        kernel[0][1] = 0;
        kernel[0][2] = 1;
        kernel[1][0] = -2;
        kernel[1][1] = 0;
        kernel[1][2] = 2;
        kernel[2][0] = -1;
        kernel[2][1] = 0;
        kernel[2][2] = 1;
    } 
    // horizontal
    else if (type == 2) {
        kernel[0][0] = -1;
        kernel[0][1] = -2;
        kernel[0][2] = -1;
        kernel[1][0] = 0;
        kernel[1][1] = 0;
        kernel[1][2] = 0;
        kernel[2][0] = 1;
        kernel[2][1] = 2;
        kernel[2][2] = 1;
    } 
    // major diagonal
    else if (type == 3) {
        kernel[0][0] = 0;
        kernel[0][1] = -1;
        kernel[0][2] = -2;
        kernel[1][0] = 1;
        kernel[1][1] = 0;
        kernel[1][2] = -1;
        kernel[2][0] = 2;
        kernel[2][1] = 1;
        kernel[2][2] = 0;
    } 
    // minor diagonal
    else {
        kernel[0][0] = -2;
        kernel[0][1] = -1;
        kernel[0][2] = 0;
        kernel[1][0] = -1;
        kernel[1][1] = 0;
        kernel[1][2] = 1;
        kernel[2][0] = 0;
        kernel[2][1] = 1;
        kernel[2][2] = 2;
    }
    
    int min_value = INT_MAX;
    int max_value = INT_MIN;

    for(int i=1; i<=rows; i++){
        for(int j=1; j<=cols; j++){
            int sum = 0;
            // apply the template
            for(int di=-1; di<=1; di++){
                for(int dj=-1; dj<=1; dj++){
                    sum += padded_img[i+di][j+dj]*kernel[di+1][dj+1];
                }
            }
            output[i-1][j-1] = sum;
            if (sum<min_value){
                min_value = sum;
            }
            if (sum>max_value){
                max_value = sum;
            }
        }
    }
    normalize(output, rows, cols, min_value, max_value, proc_img);
}

/******************************************************************/
/* Main processing routine. This is called upon pressing the      */
/* Process button of the interface.                               */
/* image  - the original greyscale image                          */
/* size   - the actual size of the image                          */
/* proc_image - the image representation resulting from the       */
/*              processing. This will be displayed upon return    */
/*              from this function.                               */
/******************************************************************/
void process_image(image, size, proc_img)
unsigned char image[DIM][DIM];
int size[2];
unsigned char proc_img[DIM][DIM];
{
    edge_detection(image, size[0], size[1], proc_img, 2);
}


