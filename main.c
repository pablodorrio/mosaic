#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "imageProcessing.h"
#include "mosaicEffect.h"

/*
 * 
 */
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
        pthread_create(&threads[i], NULL, (void *) &mosaic_thread, (void *) &filas[i]);
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

