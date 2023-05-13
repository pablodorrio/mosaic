#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <emmintrin.h>
#include <pthread.h>

#define BLOCK_SIZE 16
#define NTHREADS 4

void copyBlockSISD(int rowOr, int colOr, IplImage * imgOr, int rowDest, int colDest,
        IplImage *imgDest, int height, int width);

void copyBlockSSE2(int rowOr, int colOr, IplImage * imgOr, int rowDest, int colDest,
        IplImage *imgDest, int height, int width);

int compareBlockSISD(int row1, int column1, IplImage * img1, int row2, int column2,
        IplImage * img2, int height, int width);

int compareBlockSSE2(int row1, int column1, IplImage * img1, int row2, int column2,
        IplImage * img2, int height, int width);

IplImage* birds;
IplImage* fruits;

void mosaico_thread(void *ptr) {
    int *fila = (int *) ptr;

    int birdRow, birdColumn;
    int fruitsRow, fruitsColumn;
    int blockRow, blockColumn;
    int min, diff;

    for (birdRow = *fila * (birds->height / NTHREADS); birdRow < (*fila + 1) * (birds->height / NTHREADS); birdRow += BLOCK_SIZE) {
        for (birdColumn = 0; birdColumn < birds->width; birdColumn += BLOCK_SIZE) {
            min = BLOCK_SIZE * BLOCK_SIZE * 255 * birds->nChannels + 1;

            for (fruitsRow = 0; fruitsRow < fruits->height; fruitsRow += BLOCK_SIZE) {
                for (fruitsColumn = 0; fruitsColumn < fruits->width; fruitsColumn += BLOCK_SIZE) {
                    diff = compareBlockSSE2(birdRow, birdColumn, birds, fruitsRow, fruitsColumn, fruits, BLOCK_SIZE, BLOCK_SIZE);

                    if (diff < min) {
                        min = diff;
                        blockRow = fruitsRow;
                        blockColumn = fruitsColumn;
                    }
                }
            }
            copyBlockSSE2(blockRow, blockColumn, fruits, birdRow, birdColumn, birds, BLOCK_SIZE, BLOCK_SIZE);
        }
    }
}

int main(int argc, char** argv) {

    if (argc != 3) {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    birds = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    fruits = cvLoadImage(argv[2], CV_LOAD_IMAGE_UNCHANGED);

    // Always check if the program can find a file
    if (!birds) {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }

    if (!fruits) {
        printf("Error: fichero %s no leido\n", argv[2]);
        return EXIT_FAILURE;
    }

    cvNamedWindow(argv[1], CV_WINDOW_NORMAL);
    cvShowImage(argv[1], birds);
    cvWaitKey(0);

    cvNamedWindow(argv[2], CV_WINDOW_NORMAL);
    cvShowImage(argv[2], fruits);
    cvWaitKey(0);

    pthread_t threads[NTHREADS];

    int filas[NTHREADS];
    int i;

    for (i = 0; i < NTHREADS; i++) {
        filas[i] = i;
        printf("\nEl thread %d sustituye las filas %d", i, filas[i]);
        pthread_create(&threads[i], NULL, (void *) &mosaico_thread, (void *) &filas[i]);
    }

    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    cvNamedWindow("Mosaic", CV_WINDOW_NORMAL);
    cvShowImage("Mosaic", birds);
    cvWaitKey(0);

    // Memory release for img before exiting the application
    cvReleaseImage(&birds);
    cvReleaseImage(&fruits);

    cvDestroyWindow(argv[1]);
    cvDestroyWindow(argv[2]);
    cvDestroyWindow("Mosaic");

    return EXIT_SUCCESS;

}

void copyBlockSISD(int rowOr, int colOr, IplImage * imgOr, int rowDest, int colDest,
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

void copyBlockSSE2(int rowOr, int colOr, IplImage * imgOr, int rowDest, int colDest,
        IplImage *imgDest, int height, int width) {
    int row, column;

    for (row = 0; row < height; row++) {
        __m128i *pImg1 = (__m128i *) (imgOr->imageData + (rowOr + row) * imgOr->widthStep + colOr * imgOr->nChannels);
        __m128i *pImg2 = (__m128i *) (imgDest->imageData + (rowDest + row) * imgDest->widthStep + colDest * imgDest->nChannels);

        for (column = 0; column < width * 3; column += 16) {
            _mm_store_si128(pImg2++, _mm_load_si128(pImg1++));
        }
    }
}

int compareBlockSISD(int row1, int column1, IplImage * img1, int row2, int column2,
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

int compareBlockSSE2(int row1, int column1, IplImage * img1, int row2, int column2,
        IplImage * img2, int height, int width) {
    int row, column, toret;
    __m128i imgA, imgB, cmp, diff = _mm_setzero_si128();


    for (row = 0; row < height; row++) {
        __m128i *pImg1 = (__m128i *) (img1->imageData + column1 * img1->nChannels + (row1 + row) * img1->widthStep);
        __m128i *pImg2 = (__m128i *) (img2->imageData + column2 * img2->nChannels + (row2 + row) * img2->widthStep);

        for (column = 0; column < width * 3; column += 16) {
            imgA = _mm_loadu_si128(pImg1++);
            imgB = _mm_loadu_si128(pImg2++);
            cmp = _mm_sad_epu8(imgA, imgB);
            diff = _mm_add_epi32(diff, cmp);
        }
    }

    toret = _mm_cvtsi128_si32(diff);
    return toret;
}