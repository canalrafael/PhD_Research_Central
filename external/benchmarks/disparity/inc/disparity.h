#ifndef _DISP_
#define _DISP_

#include "sdvbs_common.h"
#include <stdint.h>
#include <stdio.h>

#define magic_timing_begin(cycleLo, cycleHi) {					\
		cycleHi = 0;											\
		asm volatile("mrs %0, cntvct_el0" : "=r"(cycleLo) );	\
	}															\

#define MAX_EXECS 1000
#define PERIOD_MS 1000

#define EVENT_TASK1 (1 << 0)
extern char img1[];
extern char img2[];

typedef struct measure_time
{
	unsigned int startCycles[2];
	unsigned int endCycles[2];
	unsigned int elapsed[2];
} measure_time_t;

static unsigned int temp;
void computeSAD(I2D *Ileft, I2D* Iright_moved, F2D* SAD);
I2D* getDisparity(I2D* Ileft, I2D* Iright, int win_sz, int max_shift);
void finalSAD(F2D* integralImg, int win_sz, F2D* retSAD);
void findDisparity(F2D* retSAD, F2D* minSAD, I2D* retDisp, int level, int nr, int nc);
void integralImage2D2D(F2D* SAD, F2D* integralImg);
void correlateSAD_2D(I2D* Ileft, I2D* Iright, I2D* Iright_moved, int win_sz, int disparity, F2D* SAD, F2D* integralImg, F2D* retSAD);
I2D* padarray2(I2D* inMat, I2D* borderMat);
void padarray4(I2D* inMat, I2D* borderMat, int dir, I2D* paddedArray);

#endif
