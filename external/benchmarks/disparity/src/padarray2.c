#include "../inc/disparity.h"

//works for 64x98 and 128x96
unsigned char spaddedArray[2*8*128*96];

//unsigned char spaddedArray[2*8*256*192];



I2D* padarray2(I2D* inMat, I2D* borderMat)
{
    int rows, cols, bRows, bCols, newRows, newCols;
    I2D *paddedArray, *Iout;
    int i, j;

    rows = inMat->height;
    cols = inMat->width;

    bRows = borderMat->data[0];
    bCols = borderMat->data[1];

    newRows = rows + bRows*2;
    newCols = cols + bCols*2;

    //paddedArray = iSetArray(newRows, newCols, 0);

    Iout = (I2D*) (spaddedArray + temp*8*128*96); //iMallocHandle(rows, cols);
    //Iout = (I2D*) (spaddedArray + temp*8*256*192);

    Iout->height = newRows;
    Iout->width  = newCols;

    for(i=0; i<newRows; i++) {
        for(j=0; j<newCols; j++) {
            subsref(Iout,i,j) = 0;
        }
    }

    paddedArray = Iout;

    temp = temp + 1;



    if(temp > 1)
        temp =0;

    //printf("temp %d \n", temp);

    for(i=0; i<rows; i++)
        for(j=0; j<cols; j++)
            subsref(paddedArray, (bRows+i), (bCols+j)) = subsref(inMat, i, j);

    return paddedArray;

}
