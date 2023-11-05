#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>
#include <stdlib.h> 

#define DIM 512

/******************************************************************/
/* This structure contains the coordinates of a box drawn with    */
/* the left mouse button on the image window.                     */
/* roi.x , roi.y  - left upper corner's coordinates               */
/* roi.width , roi.height - width and height of the box           */
/******************************************************************/
extern XRectangle roi;

int hashmap[DIM * DIM] = {0};

int findRoot(int idx) {
    while (hashmap[idx] != 0) {
        idx = hashmap[idx];
    }
    return idx;
}

int countBlock(unsigned char proc_img[DIM][DIM], int rows, int cols){
    int res = 0;
    int set[DIM*DIM] = {0};

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int block = proc_img[i][j];
            if (set[block] == 0) {
                res++;
                set[block] = 1;
            }
        }
    }
    
    return res;
}

void blob_coloring(unsigned char image[DIM][DIM], int rows, int cols, unsigned char proc_img[DIM][DIM], int thresh){
    int block = 0;
    
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            bool above = false;
            bool left = false;

            // check left cell
            if(j-1>=0 && abs(image[i][j]-image[i][j-1])<=thresh){
                proc_img[i][j] = proc_img[i][j-1];
                left = true;
            }

            // check above cell
            if(i-1>=0 && abs(image[i][j]-image[i-1][j])<=thresh){
                proc_img[i][j] = proc_img[i-1][j];
                above = true;
            }
            

            if(above && left){
                if(proc_img[i-1][j] == proc_img[i][j-1]){
                    continue;
                }

                //link block
                if(proc_img[i-1][j] < proc_img[i][j-1]){
                    hashmap[proc_img[i][j-1]] = proc_img[i-1][j];
                }else{
                    hashmap[proc_img[i-1][j]] = proc_img[i][j-1];
                }
                
            }
            // if either case is correct, then move on
            else if(above || left){
                continue;
            }
            // if both case is wrong, then assign new color
            else if (!above && !left) 
            {
                block +=1;
                proc_img[i][j] = block;
            }
        }
    }

    //for debugging
    for(int i=0; i<=block; i++){
        printf("%d\n", hashmap[i]);
    }

    // link block loop
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            proc_img[i][j] = findRoot(proc_img[i][j]);
        }
    }   
    int numBlock = countBlock(proc_img, rows, cols);
    printf("number of blocks: %d\n", numBlock);
}

int main() {
    unsigned char image[DIM][DIM] = {{0, 0, 0, 0, 0, 0, 0},
                                     {0, 1, 1, 0, 1, 1, 0},
                                     {0, 1, 1, 0, 1, 0, 0},
                                     {0, 0, 1, 1, 1, 0, 0},
                                     {0, 0, 0, 1, 1, 0, 0},
                                     {0, 1, 1, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0}};

    unsigned char proc_img[DIM][DIM];
    int rows = 7;
    int cols = 7;
    int thresh = 0;

    blob_coloring(image, rows, cols, proc_img, thresh);

    // Print the resulting labeled image
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", proc_img[i][j]);
        }
        printf("\n");
    }

    return 0;
}


