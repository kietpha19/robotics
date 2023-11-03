#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <limits.h>
#include <float.h>

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

void get_template(unsigned char image[DIM][DIM], int template[roi.height][roi.width], double* template_mean, double* template_std){
    double sum = 0;
    for(int i=0; i<roi.height; i++){
        for(int j=0; j<roi.width; j++){
            template[i][j] = image[i+roi.y][j+roi.x];
            sum += template[i][j];
        }
    }
    *template_mean = (double)(sum/(roi.height*roi.width));

    sum = 0;
    for(int i=0; i<roi.height; i++){
        for(int j=0; j<roi.width; j++){
            sum += (double) ((template[i][j]- (*template_mean)) * (template[i][j]- (*template_mean)));
        }
    }
    *template_std = sqrt(sum/(roi.height*roi.width));
}

double get_mean(unsigned char image[DIM][DIM], int i, int j){
    double sum = 0;
    for(int r=0; r<roi.height; r++){
        for(int c=0; c<roi.width; c++){
            sum += image[r+i][c+j];
        }
    }
    return (double) (sum/(roi.height*roi.width));
}

double get_std(unsigned char image[DIM][DIM], int i,int j, double mean){
    double sum = 0;
    for(int r=0; r<roi.height; r++){
        for(int c=0; c<roi.width; c++){
            sum += (image[r+i][c+j]-mean)*(image[r+i][c+j]-mean);
        }
    }
    return sqrt(sum/(roi.height*roi.width));
}

void template_matching(unsigned char image[DIM][DIM], int rows, int cols, unsigned char proc_img[DIM][DIM]){
    int template[roi.height][roi.width];
    double template_mean;
    double template_std;
    get_template(image, template, &template_mean, &template_std);
    
    // printf("%d\n", template_mean);
    // printf("%d\n", template_std);

    double output[DIM][DIM];
    double min_value = DBL_MAX;
    double max_value = DBL_MIN;
    
   
    for(int i=0; i<rows-roi.height+1; i++){
        for(int j=0; j<cols-roi.width+1; j++){
            double mean = get_mean(image, i, j);
            double std = get_std(image, i, j, mean);
            //apply template
            double sum = 0;
            for(int di=0; di<roi.height; di++){
                for(int dj=0; dj<roi.width; dj++){
                    sum += (image[i+di][j+dj]-mean)*(template[di][dj]-template_mean)/(std*template_std);
                }
            }
            double value = sum/(roi.height*roi.width);
            output[i][j] = value;

            if (value < min_value) {
                min_value = value;
            }
            if (value > max_value) {
                max_value = value;
            }
        }
    }

    // normalize
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            proc_img[i][j] = (unsigned char)(((output[i][j] - min_value) / (max_value - min_value)) * 255);
        }
    }
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
    template_matching(image, size[0], size[1], proc_img);
    // printf("%d\n", roi.x);
    // printf("%d\n", roi.y);
    
}



