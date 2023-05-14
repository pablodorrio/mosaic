#include "mosaicEffect.h"

void mosaic_thread(void *ptr) {
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