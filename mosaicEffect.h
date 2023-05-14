#pragma once

#include "imageProcessing.h"

#define BLOCK_SIZE 16
#define NTHREADS 4

IplImage* birds;
IplImage* fruits;

void mosaic_thread(void *ptr);