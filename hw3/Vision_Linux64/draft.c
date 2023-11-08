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

    double min_value = DBL_MAX;
    double max_value = DBL_MIN;

    // link block loop
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            int value = findRoot(proc_img[i][j]);
            proc_img[i][j] = value;

            if (value < min_value) {
                min_value = value;
            }
            if (value > max_value) {
                max_value = value;
            }
        }
    }   

    // normalize
    // for (int i = 0; i < rows; i++) {
    //     for (int j = 0; j < cols; j++) {
    //         proc_img[i][j] = (unsigned char)(((proc_img[i][j] - min_value) / (max_value - min_value)) * 255);
    //     }
    // }
    int numBlock = countBlock(proc_img, rows, cols);
    printf("number of blocks: %d\n", numBlock);
}

int main() {
    // unsigned char image[DIM][DIM] = {{0, 0, 0, 0, 0, 0, 0},
    //                                  {0, 1, 1, 0, 1, 1, 0},
    //                                  {0, 1, 1, 0, 1, 0, 0},
    //                                  {0, 0, 1, 1, 1, 0, 0},
    //                                  {0, 0, 0, 1, 1, 0, 0},
    //                                  {0, 1, 1, 0, 0, 0, 0},
    //                                  {0, 0, 0, 0, 0, 0, 0}};

    unsigned char image[DIM][DIM] = {{1, 2, 3, 4, 5, 6, 7},
                                     {0, 1, 1, 0, 1, 1, 0},
                                     {0, 1, 1, 0, 1, 0, 0},
                                     {0, 0, 1, 1, 1, 0, 0},
                                     {0, 0, 0, 1, 1, 0, 0},
                                     {0, 1, 1, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0}};

    unsigned char proc_img[DIM][DIM];
    int rows = 7;
    int cols = 7;
    int thresh = 1;

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


/*
int label_img[DIM][DIM];
int output_img[DIM][DIM];

// Function to find the minimum value in an array
int findMin(int array[], int size) {
    int min = array[0];
    for (int i = 1; i < size; i++) {
        if (array[i] < min) {
            min = array[i];
        }
    }
    return min;
}

// Function to find the maximum value in an array
int findMax(int array[], int size) {
    int max = array[0];
    for (int i = 1; i < size; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    return max;
}

void process_image(unsigned char input_image[DIM][DIM], int size[2], unsigned char proc_img[DIM][DIM]) {
    int num_regions = 0;
    int labels[DIM * DIM] = {0};

    for (int r = 0; r < size[1]; r++) {
        for (int c = 0; c < size[0]; c++) {
            int prev_r = (r - 1 >= 0) ? input_image[c][r - 1] : 0;
            int prev_c = (c - 1 >= 0) ? input_image[c - 1][r] : 0;

            if (abs(input_image[c][r] - prev_r) <= THRESHOLD &&
                abs(input_image[c][r] - prev_c) <= THRESHOLD) {
                if (label_img[c][r - 1] == label_img[c - 1][r]) {
                    label_img[c][r] = label_img[c][r - 1];
                } else {
                    int smaller = label_img[c][r - 1];
                    int bigger = label_img[c - 1][r];
                    label_img[c][r] = (smaller < bigger) ? smaller : bigger;
                }
            } else if (abs(input_image[c][r] - prev_r) <= THRESHOLD) {
                label_img[c][r] = label_img[c][r - 1];
            } else if (abs(input_image[c][r] - prev_c) <= THRESHOLD) {
                label_img[c][r] = label_img[c - 1][r];
            } else {
                label_img[c][r] = num_regions;
                labels[num_regions] = num_regions;
                num_regions++;
            }
        }
    }

    for (int r = 0; r < size[1]; r++) {
        for (int c = 0; c < size[0]; c++) {
            int match_count = 0;
            int current_label = label_img[c][r];
            for (int i = 0; i < num_regions; i++) {
                if (current_label == labels[i]) {
                    output_img[c][r] = i;
                    current_label = i;
                    match_count++;
                }
            }
            if (match_count == 0) {
                output_img[c][r] = label_img[c][r];
            }
        }
    }

    int num_unique_regions = 1;
    int unique_labels[DIM * DIM] = {0};
    unique_labels[0] = output_img[0][0];

    for (int r = 0; r < size[1]; r++) {
        for (int c = 0; c < size[0]; c++) {
            bool match = false;
            int j = 0;
            while (!match && j < num_unique_regions) {
                if (output_img[c][r] == unique_labels[j]) {
                    match = true;
                }
                j++;
            }
            if (!match) {
                unique_labels[num_unique_regions] = output_img[c][r];
                num_unique_regions++;
            }
        }
    }

    int min_label = findMin(unique_labels, num_unique_regions);
    int max_label = findMax(unique_labels, num_unique_regions);

    double m = (255.0 - 30.0) / (max_label - min_label);
    double b = m * -min_label;
    int intensities[DIM * DIM] = {0};

    intensities[unique_labels[0]] = 0;

    for (int j = 1; j < num_unique_regions; j++) {
        intensities[(int)round(m * unique_labels[j] + b)] = j * (255 - 30) / num_unique_regions + 30;
    }

    for (int r = 0; r < size[1]; r++) {
        for (int c = 0; c < size[0]; c++) {
            proc_img[c][r] = intensities[(int)round(m * output_img[c][r] + b)];
        }
    }
}


*/