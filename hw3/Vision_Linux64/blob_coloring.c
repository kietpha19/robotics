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

int label_img[DIM][DIM];
int output_img[DIM][DIM];

int parent[DIM * DIM];
int rank[DIM * DIM];

// Find operation for the union-find data structure
int find(int x) {
    if (x != parent[x]) {
        parent[x] = find(parent[x]);
    }
    return parent[x];
}

// Union operation for the union-find data structure
void unionSets(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);

    if (rootX != rootY) {
        if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
    }
}

void blob_coloring(unsigned char input_image[DIM][DIM], int rows, int cols, unsigned char proc_img[DIM][DIM], int thresh) {
    int num_regions = 0;

    // Initialize the parent and rank arrays for the union-find data structure
    for (int i = 0; i < DIM * DIM; i++) {
        parent[i] = i;
        rank[i] = 0;
    }

    // first loop (assign label pass)
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int prev_r = (r - 1 >= 0) ? input_image[c][r - 1] : 0;
            int prev_c = (c - 1 >= 0) ? input_image[c - 1][r] : 0;

            if (abs(input_image[c][r] - prev_r) <= thresh &&
                abs(input_image[c][r] - prev_c) <= thresh) {
                if (label_img[c][r - 1] == label_img[c - 1][r]) {
                    label_img[c][r] = label_img[c][r - 1];
                } else {
                    int smaller = label_img[c][r - 1];
                    int bigger = label_img[c - 1][r];
                    label_img[c][r] = (smaller < bigger) ? smaller : bigger;
                    unionSets(label_img[c][r - 1], label_img[c - 1][r]); // Union the sets
                }
            } else if (abs(input_image[c][r] - prev_r) <= thresh) {
                label_img[c][r] = label_img[c][r - 1];
            } else if (abs(input_image[c][r] - prev_c) <= thresh) {
                label_img[c][r] = label_img[c - 1][r];
            } else {
                label_img[c][r] = num_regions;
                num_regions++;
            }
        }
    }

    // link blobs loop (dictionary mapping pass)
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            output_img[c][r] = find(label_img[c][r]); // Find the representative of the set
        }
    }

    int min_label = num_regions;
    int max_label = 0;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int label = output_img[c][r];
            if (label < min_label) {
                min_label = label;
            }
            if (label > max_label) {
                max_label = label;
            }
        }
    }

    // normalize and adjust the constrast so that the output img is easier to see
    double m = (255.0 - 80.0) / (max_label - min_label);
    double b = m * -min_label;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            proc_img[c][r] = (int)round(m * output_img[c][r] + b);
        }
    }

}

void process_image(image, size, proc_img)
unsigned char image[DIM][DIM];
int size[2];
unsigned char proc_img[DIM][DIM];
{
    int thresh;
    printf("threshhold: ");
    scanf("%d", &thresh);
    
    blob_coloring(image, size[1], size[0], proc_img, thresh);
    printf("finished\n");
    //print_img(proc_img, size[0], size[1]);
}
