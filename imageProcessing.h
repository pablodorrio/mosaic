#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <emmintrin.h>

void copyBlockSISD(int rowOr, int colOr, IplImage * imgOr, int rowDest, int colDest,
        IplImage *imgDest, int height, int width);

void copyBlockSSE2(int rowOr, int colOr, IplImage * imgOr, int rowDest, int colDest,
        IplImage *imgDest, int height, int width);

int compareBlockSISD(int row1, int column1, IplImage * img1, int row2, int column2,
        IplImage * img2, int height, int width);

int compareBlockSSE2(int row1, int column1, IplImage * img1, int row2, int column2,
        IplImage * img2, int height, int width);