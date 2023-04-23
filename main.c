#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

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

    // Memory release for img before exiting the application
    cvReleaseImage(&bird);
    cvReleaseImage(&fruits);

    cvDestroyWindow(argv[1]);
    cvDestroyWindow(argv[2]);

    return EXIT_SUCCESS;

}
