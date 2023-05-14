#include "imageProcessing.h"

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