#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>


void copyBlock(int rowOr, int colOr, IplImage * imgOr, int rowDest, int colDest,
        IplImage *imgDest, int height, int width);

int compareBlock(int row1, int column1, IplImage * img1, int row2, int column2,
        IplImage * img2, int height, int width);

int main(int argc, char** argv) {

    if (argc != 3) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    IplImage* bird = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    IplImage* fruits = cvLoadImage(argv[2], CV_LOAD_IMAGE_UNCHANGED);

    // Always check if the program can find a file
    if (!bird) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }

    if (!fruits) {
        printf("Error: fichero %s no leido\n", argv[2]);
        return EXIT_FAILURE;
    }

    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvShowImage(argv[1], bird);
    cvWaitKey(0);

    cvNamedWindow(argv[2], CV_WINDOW_NORMAL);
    cvShowImage(argv[2], fruits);
    cvWaitKey(0);

    int birdRow, birdColumn;
    int fruitsRow, fruitsColumn;
    int blockRow, blockColumn;
    int min, diff;

    for (birdRow = 0; birdRow < bird->height; birdRow += 16) {
        for (birdColumn = 0; birdColumn < bird->width; birdColumn += 16) {
            min = 16 * 16 * 255 * bird->nChannels + 1;

            for (fruitsRow = 0; fruitsRow < fruits->height; fruitsRow += 16) {
                for (fruitsColumn = 0; fruitsColumn < fruits->width; fruitsColumn += 16) {
                    diff = compareBlock(birdRow, birdColumn, bird, fruitsRow, fruitsColumn, fruits, 16, 16);

                    if (diff < min) {
                        min = diff;
                        blockRow = fruitsRow;
                        blockColumn = fruitsColumn;
                    }
                }
            }
            copyBlock(blockRow, blockColumn, fruits, birdRow, birdColumn, bird, 16, 16);
        }
    }

    cvNamedWindow("Mosaic", CV_WINDOW_NORMAL);
    cvShowImage("Mosaic", bird);
    cvWaitKey(0);

    // Memory release for img before exiting the application
    cvReleaseImage(&bird);
    cvReleaseImage(&fruits);

    cvDestroyWindow(argv[1]);
    cvDestroyWindow(argv[2]);
    cvDestroyWindow("Mosaic");

    return EXIT_SUCCESS;

}

void copyBlock(int rowOr, int colOr, IplImage * imgOr, int rowDest, int colDest,
        IplImage *imgDest, int height, int width) {
    int row, column;

    for (row = 0; row < height; row++) {
        unsigned char *pImg1 = (unsigned char *) (imgOr->imageData + (rowOr + row) * imgOr->widthStep + colOr * imgOr->nChannels);
        unsigned char *pImg2 = (unsigned char *) (imgDest->imageData + (rowDest + row) * imgDest->widthStep + colDest * imgDest->nChannels);

        for (column = 0; column < width; column++) {
            *pImg2++ = *pImg1++;
            *pImg2++ = *pImg1++;
            *pImg2++ = *pImg1++;
        }
    }

}

int compareBlock(int row1, int column1, IplImage * img1, int row2, int column2,
        IplImage * img2, int height, int width) {
    int row, column;
    int cc1Diff, cc2Diff, cc3Diff, blockDiff = 0;

    for (row = 0; row < height; row++) {
        unsigned char * pImg1 = (unsigned char *) (img1->imageData + column1 * img1->nChannels + (row1 + row) * img1->widthStep);
        unsigned char * pImg2 = (unsigned char *) (img2->imageData + column2 * img2->nChannels + (row2 + row) * img2->widthStep);

        for (column = 0; column < width; column++) {
            cc1Diff = abs(*pImg1++ - *pImg2++);
            cc2Diff = abs(*pImg1++ - *pImg2++);
            cc3Diff = abs(*pImg1++ - *pImg2++);

            blockDiff += cc1Diff + cc2Diff + cc3Diff;
        }
    }

    return blockDiff;
}