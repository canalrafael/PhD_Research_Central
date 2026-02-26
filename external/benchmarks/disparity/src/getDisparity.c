#include "../inc/disparity.h"


unsigned char sminSAD[8 + 4*64*48];
unsigned char sretDisp[8 + 4*64*48];
unsigned char shalfWin[8 + 4*64*48];
unsigned char sSAD[8 + 8*64*48];
unsigned char sintergalImg[8 + 8*64*48];
unsigned char sIright_moved[8 + 8*64*48];
unsigned char sretSAD[8 + 8*64*48];

I2D* getDisparity(I2D* Ileft, I2D* Iright, int win_sz, int max_shift)
{


    I2D* retDisp;
    int nr, nc, k;
    I2D *halfWin;
    int half_win_sz, rows, cols;
    F2D *retSAD, *minSAD, *SAD, *integralImg;
    I2D* IrightPadded, *IleftPadded, *Iright_moved, *Iout;

    int i, j;
    F2D *fout;


    nr = Ileft->height;
    nc = Ileft->width;
    half_win_sz=win_sz/2;




    //minSAD = fSetArray(nr, nc, 255.0*255.0); // This fuck has malloc so let us replace it.


    fout = (F2D *)sminSAD;
    fout->height = nr;
    fout->width = nc;
    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            subsref(fout,i,j) = 255*255;
        }
    }

    minSAD = fout;


    // Replace retDisp
    //retDisp = iSetArray(nr, nc,max_shift);

    Iout = (I2D *) sretDisp; //iMallocHandle(rows, cols);

    Iout->height = nr;
    Iout->width = nc;

    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            subsref(Iout,i,j) = max_shift;
        }
    }

    retDisp = Iout;
//=================================================================================//

   // halfWin = iSetArray(1,2,half_win_sz);

    Iout = (I2D *)shalfWin; //iMallocHandle(rows, cols);

    Iout->height = nr;
    Iout->width = nc;

    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            subsref(Iout,i,j) = half_win_sz;
        }
    }

    halfWin = Iout;

//================================================================================//

    if(win_sz > 1)
    {
        IleftPadded = padarray2(Ileft, halfWin); // This fuck as malloc as well
        IrightPadded = padarray2(Iright, halfWin); // This weird guy wrote everything with malloc
    }
    else
    {
        IleftPadded = Ileft;
        IrightPadded = Iright;
    }

    rows = IleftPadded->height;
    cols = IleftPadded->width;

    //SAD = fSetArray(rows, cols,255);

    fout = (F2D *)sSAD;
    fout->height = rows;
    fout->width = cols;
    for(i=0; i<rows; i++) {
        for(j=0; j<cols; j++) {
            subsref(fout,i,j) = 255;
        }
    }

    SAD = fout;


//===============================================================================
    //integralImg = fSetArray(rows, cols,0);

    fout = (F2D *)sintergalImg;
    fout->height = rows;
    fout->width = cols;
    for(i=0; i<rows; i++) {
        for(j=0; j<cols; j++) {
            subsref(fout,i,j) = 0;
        }
    }

    integralImg = fout;


//===============================================================================

    //retSAD = fMallocHandle(rows-win_sz, cols-win_sz);
    retSAD =  (F2D *)sretSAD;

    retSAD->height = rows-win_sz;
    retSAD->width  = cols-win_sz;

//=====================================================================================
    //Iright_moved = iSetArray(rows, cols, 0);

    Iout = (I2D *)sIright_moved; //iMallocHandle(rows, cols);

    Iout->height = rows;
    Iout->width = cols;

    for(i=0; i<rows; i++) {
        for(j=0; j<cols; j++) {
            subsref(Iout,i,j) = 0;
        }
    }

    Iright_moved = Iout;


    for( k=0; k<max_shift; k++)
    {
        correlateSAD_2D(IleftPadded, IrightPadded, Iright_moved, win_sz, k, SAD, integralImg, retSAD); // Removed Mallocs here
        findDisparity(retSAD, minSAD, retDisp, k, nr, nc); // No malloc here
    }

    return retDisp;
}
