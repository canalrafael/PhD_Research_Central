#include "../inc/disparity.h"

void correlateSAD_2D(I2D* Ileft, I2D* Iright, I2D* Iright_moved, int win_sz, int disparity, F2D* SAD, F2D* integralImg, F2D* retSAD)
{
    int rows, cols;
    int i;
    I2D *range;
    unsigned char sRange[8 + sizeof(int)*1*2];

    //range = iMallocHandle(1,2);
    range = (I2D*) sRange;
    subsref(range,0,0) = 0;
    subsref(range,0,1) = disparity; // no malloc

    rows = Iright_moved->height;
    cols = Iright_moved->width;

    //printf("rows = %d col = %d \n", rows,cols);

    for(i=0; i<rows*cols; i++)
        asubsref(Iright_moved,i) = 0; // No Malloc



    padarray4(Iright, range, -1, Iright_moved); // No Malloc here as well good



    computeSAD(Ileft, Iright_moved, SAD); // No Malloc in this fuck also
    integralImage2D2D(SAD, integralImg); // No malloc in this fuck
    finalSAD(integralImg, win_sz, retSAD); // No Malloc in this fuck as well


    return;
}
