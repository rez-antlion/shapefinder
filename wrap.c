#include "wrap.h"
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define UINT8_MAX_VALUE (255)

Complex uintToComplex(uint8 u) {
	Complex c;
	c.re = (float) u;
	c.im = 0.;
	return c;
}

Complex *tab1UintToComplex(uint8* tabUint, int size) {
	int i;
	Complex *tabComplex;
	tabComplex = (Complex *) malloc(size * sizeof(Complex));
	
	
	for (i = 0; i < size; i++) {
		tabComplex[i] = uintToComplex(tabUint[i]);
	}
	
	return tabComplex;
}

Complex **tab2UintToComplex(uint8 **tab2Uint, int height, int width) {
	int i;
	Complex **tabComplex;
	tabComplex = (Complex **) malloc(height * sizeof(Complex*));
	
	for (i = 0; i < height; i++) {
		tabComplex[i] = tab1UintToComplex(tab2Uint[i], width);
	}	
	return tabComplex;
}

void iplImageToBwimage(bwimage_t **target, IplImage *src) {
    int i, j, height, width;
    *target = (bwimage_t *) malloc (sizeof(bwimage_t));
    height = (*target)->height = src->height;
    width = (*target)->width = src->width;
    (*target)->rawdata = (uint8 *) malloc(height * width * sizeof(uint8));
    (*target)->data = (uint8 **) malloc(height * sizeof(uint8 *));
    for (i = 0; i < height; i++) {
        (*target)->data[i] = (uint8 *) malloc(width * sizeof(uint8));
        for (j = 0; j < width; j++) {
            (*target)->data[i][j] = (*target)->rawdata[i * width + j] 
                                  = src->imageData[i * width + j];
        }
    }
}

cimage_t *bwimageToCimage(bwimage_t *image) {
	cimage_t *complexImage;
	int height = image->height, width = image->width, i, j;

	complexImage = (cimage_t*) malloc (sizeof(cimage_t));	
	complexImage->height = height;
	complexImage->width = width;
	complexImage->rawdata =
                (Complex *) malloc(height * width * sizeof(Complex));
	complexImage->data =
                (Complex **) malloc(height * sizeof(Complex*));
	for (i = 0; i < height; i++) {
		complexImage->data[i] =
                        (Complex *) malloc(width * sizeof(Complex));
		for (j = 0; j < width; j++) {
			complexImage->data[i][j] =
                        complexImage->rawdata[i * width + j] =
                        uintToComplex(image->rawdata[i * width + j]);
		}
	}
	return complexImage;
}

bwimage_t *cimageToBwimage(cimage_t *complexImage) {
	bwimage_t *image;
	int height = complexImage->height, width = complexImage->width, i, j;
	float max, min;
	
	image = (bwimage_t*) malloc (sizeof(bwimage_t));	
	image->height = height;
	image->width = width;
	image->rawdata = (uint8 *) malloc(height * width * sizeof(uint8));
	image->data = (uint8 **) malloc(height * sizeof(uint8*));
	maxMinTab1Complex(complexImage->rawdata, height * width, &max, &min);
	for (i = 0; i < height; i++) {
		image->data[i] = (uint8 *) malloc(width * sizeof(uint8));
		for (j = 0; j < width; j++) {
			image->data[i][j] = image->rawdata[i * width + j] = 
                        (uint8) ((complexImage->rawdata[i * width + j].re - min)
                                        * UINT8_MAX_VALUE / (max - min) + 0.5);
		}
	}
	return image;
}

bwimage_t *cimageToBwimageMemorizePeaks(cimage_t *complexImage,
        float tolerancethreshold, int *npeak, Pixel *maximizer) {
	bwimage_t *image;
	int height = complexImage->height, width = complexImage->width, i, j;
	float max, min;
	
	image = (bwimage_t*) malloc (sizeof(bwimage_t));	
	image->height = height;
	image->width = width;
	image->rawdata = (uint8 *) malloc(height * width * sizeof(uint8));
	image->data = (uint8 **) malloc(height * sizeof(uint8*));
	maxMinTab1Complex(complexImage->rawdata, height * width, &max, &min);
	locatePeaks(complexImage->data, height, width, max, tolerancethreshold,
                        npeak, maximizer);
	for (i = 0; i < height; i++) {
		image->data[i] = (uint8 *) malloc(width * sizeof(uint8));
		for (j = 0; j < width; j++) {
			image->data[i][j] = image->rawdata[i * width + j]
                        =(uint8)((complexImage->rawdata[i * width + j].re - min)
                                         * UINT8_MAX_VALUE / (max - min) + 0.5);
		}
	}
	return image;
}

void copyImage(bwimage_t *dest, bwimage_t *src) {
    int i,j;
    for (i = 0; i < src->height; i++) {
        for (j = 0; j < src->width; j++) {
            dest->data[i][j] = dest->rawdata[i * src->width + j]
                             = src->data[i][j];
        }
    }
}

cimage_t *createCimage(int height, int width) {
	cimage_t *complexImage;
	int i;
	complexImage = (cimage_t *) malloc(sizeof(cimage_t));
	complexImage->height = height;
	complexImage->width = width;
	complexImage->rawdata = (Complex *) malloc(sizeof(Complex)
                                                    * height * width);
	complexImage->data = (Complex **) malloc(sizeof(Complex *) * height);
	for (i=0; i < height; i++) {
		(complexImage->data)[i] =
                        (Complex *)malloc(sizeof(Complex) * width);
	}
	return complexImage;
}

bwimage_t *createBlackBwimage(int height, int width) {
	bwimage_t *image;
	int i, j;
	image = (bwimage_t *) malloc(sizeof(bwimage_t));
	image->height = height;
	image->width = width;
	image->rawdata = (uint8 *) malloc(sizeof(uint8) * height * width);
	image->data = (uint8 **) malloc(sizeof(uint8 *) * height);
	for (i=0; i < height; i++) {
		(image->data)[i] = (uint8 *) malloc(sizeof(Complex) * width);
		for (j=0; j < width; j++) {
			image->data[i][j] = (uint8)0;
		}
	}
	return image;
}

bwimage_t *createWhiteBwimage(int height, int width) {
	bwimage_t *image;
	int i, j;
	image = (bwimage_t *) malloc(sizeof(bwimage_t));
	image->height = height;
	image->width = width;
	image->rawdata = (uint8 *) malloc(sizeof(uint8) * height * width);
	image->data = (uint8 **) malloc(sizeof(uint8 *) * height);
	for (i=0; i < height; i++) {
		(image->data)[i] = (uint8 *) malloc(sizeof(Complex) * width);
		for (j=0; j < width; j++) {
			image->data[i][j] = UINT8_MAX_VALUE;
		}
	}
	return image;
}

Complex complexMultiply(Complex c1, Complex c2) {
	Complex result;
	result.re = c1.re * c2.re - c1.im * c2.im;
	result.im = c1.im * c2.re + c1.re * c2.im;
	return result;
}

Complex complexConjugate(Complex c) {
	c.im = -c.im;
	return c;
}

uint8 *tab1ComplexToUint(Complex *tab1Complex, int size) {
	float max, min;
	int i;
	uint8 *tab1Uint = (uint8 *) malloc(size * sizeof(uint8));
	maxMinTab1Complex(tab1Complex, size, &max, &min);
	for (i=0; i < size; i++) {
		tab1Uint[i] = (uint8) ((tab1Complex[i].re - min)
                                * UINT8_MAX_VALUE / (max - min) + 0.5);
	}
	return tab1Uint;
}

uint8 **tab2ComplexToUint(Complex **tab2Complex, int height, int width) {
	float max, min;
	int i, j;
	uint8 **tab2Uint = (uint8 **) malloc(height * sizeof(uint8 *));
	maxMinTab2Complex(tab2Complex, height, width, &max, &min);
	for (i=0; i < height; i++) {
		tab2Uint[i] = (uint8 *) malloc(width * sizeof(uint8));
		for (j=0; j < width; j++) {
			tab2Uint[i][j] = (uint8) ((tab2Complex[i][j].re - min)
                                * UINT8_MAX_VALUE / (max - min) + 0.5);
		}
	}
	return tab2Uint;	
}

float *tab1ComplexToFloat(Complex *tab1Complex, int size) {
	int i;
	float *tab1Float = (float *) malloc(size * sizeof(float));
	for (i=0; i < size; i++) {
		tab1Float[i] = tab1Complex[i].re;
	}
	return tab1Float;
}

float **tab2ComplexToFloat(Complex **tab2Complex, int height, int width) {
	int i;
	float **tab2Float = (float **) malloc(height * sizeof(float *));
	for (i=0; i < height; i++) {
		tab2Float[i] = tab1ComplexToFloat(tab2Complex[i], width);
	}
	return tab2Float;
}

void maxMinTab1Float(float *tab1Float, int size, float *max, float *min) {
	int i;
	*max = *min = tab1Float[0];
	for (i=1; i < size; i++) {
		if (tab1Float[i] > *max) {
			*max = tab1Float[i];
		}
		if (tab1Float[i] < *min) {
			*min = tab1Float[i];
		}
	}
}

void maxMinTab2Float(float **tab2Float, int height, int width,
                                        float *max, float *min) {
	int i, j;
	*max = *min = tab2Float[0][0];
	for (i=0; i < height; i++) {
		for (j=1; j < width; j++) {
			if (tab2Float[i][j] > *max) {
				*max = tab2Float[i][j];
			}
			if (tab2Float[i][j] < *min) {
				*min = tab2Float[i][j];
			}
		}
	}
}

void maxMinTab1Complex(Complex *tab1Complex, int size, float *max, float *min) {
	int i;
	*max = *min = tab1Complex[0].re;
	for (i=1; i < size; i++) {
		if (tab1Complex[i].re > *max) {
			*max = tab1Complex[i].re;
		}
		if (tab1Complex[i].re < *min) {
			*min = tab1Complex[i].re;
		}
	}
}

void maxMinTab2Complex(Complex **tab2Complex, int height,
                        int width, float *max, float *min) {
	int i, j;
	*max = *min = tab2Complex[0][0].re;
	for (i=0; i < height; i++) {
		for (j=1; j < width; j++) {
			if (tab2Complex[i][j].re > *max) {
				*max = tab2Complex[i][j].re;
			}
			if (tab2Complex[i][j].re < *min) {
				*min = tab2Complex[i][j].re;
			}
		}
	}
}

void locatePeaks(Complex **tab2Complex, int height, int width, float max,
                 float tolerancethreshold, int *npeak, Pixel *maximizer) {
	int i, j;
	*npeak = 0;
	for (i=0; i < height; i++) {
		for (j=0; j < width; j++) {
			if (tab2Complex[i][j].re >= tolerancethreshold*max) {
				(*npeak)++;
				maximizer[(*npeak) - 1].x = j;
				maximizer[(*npeak) - 1].y = i;
			}
		}
	}
}

void freeCimage(cimage_t *complexImage) {
	freeTab1Complex(complexImage->rawdata);
	freeTab2Complex(complexImage->data, complexImage->height);
	free(complexImage);
}

void freeBwimage(bwimage_t *image) {
	freeTab1Uint(image->rawdata);
	freeTab2Uint(image->data, image->height);
	free(image);
}

void freeTab2Complex(Complex **tab2Complex, int height) {
	int i;
	for (i=0; i < height; i++) {
		free(tab2Complex[i]);
	}
	free(tab2Complex);
}
void freeTab2Float(float **tab2Float, int height) {
	int i;
	for (i=0; i < height; i++) {
		free(tab2Float[i]);
	}
	free(tab2Float);
}
void freeTab2Uint(uint8 **tab2Uint, int height) {
	int i;
	for (i=0; i < height; i++) {
		free(tab2Uint[i]);
	}
	free(tab2Uint);
}
void freeTab1Complex(Complex* tab1Complex) {
	free(tab1Complex);
}
void freeTab1Float(float *tab1Float) {
	free(tab1Float);
}
void freeTab1Uint(uint8 *tab1Uint) {
	free(tab1Uint);
}

void printTab1Complex(Complex* tabComplex, int size) {
	int i;
	for (i = 0; i < size; i++) {
                printf("%10.2f + i*%10.2f\t",tabComplex[i].re,tabComplex[i].im);
	}
}

void fourn(float data[], unsigned long nn[], int ndim, int isign) {

    int idim;
    unsigned long i1, i2, i3, i2rev, i3rev, ip1, ip2, ip3, ifp1, ifp2;
    unsigned long ibit, k1, k2, n, nprev, nrem, ntot;
    float tempi, tempr;
    double theta, wi, wpi, wpr, wr, wtemp;

    data--;
    nn--;

    for (ntot = 1, idim = 1; idim <= ndim; idim++)
        ntot *= nn[idim];
    nprev = 1;
    for (idim = ndim; idim >= 1; idim--) {
        n = nn[idim];
        nrem = ntot / (n * nprev);
        ip1 = nprev << 1;
        ip2 = ip1*n;
        ip3 = ip2*nrem;
        i2rev = 1;
        for (i2 = 1; i2 <= ip2; i2 += ip1) {
            if (i2 < i2rev) {
                for (i1 = i2; i1 <= i2 + ip1 - 2; i1 += 2) {
                    for (i3 = i1; i3 <= ip3; i3 += ip2) {
                        i3rev = i2rev + i3 - i2;
                        SWAP(data[i3], data[i3rev]);
                        SWAP(data[i3 + 1], data[i3rev + 1]);
                    }
                }
            }
            ibit = ip2 >> 1;
            while (ibit >= ip1 && i2rev > ibit) {
                i2rev -= ibit;
                ibit >>= 1;
            }
            i2rev += ibit;
        }
        ifp1 = ip1;
        while (ifp1 < ip2) {
            ifp2 = ifp1 << 1;
            theta = isign * 6.28318530717959 / (ifp2 / ip1);
            wtemp = sin(0.5 * theta);
            wpr = -2.0 * wtemp*wtemp;
            wpi = sin(theta);
            wr = 1.0;
            wi = 0.0;
            for (i3 = 1; i3 <= ifp1; i3 += ip1) {
                for (i1 = i3; i1 <= i3 + ip1 - 2; i1 += 2) {
                    for (i2 = i1; i2 <= ip3; i2 += ifp2) {
                        k1 = i2;
                        k2 = k1 + ifp1;
                        tempr = (float) wr * data[k2]-(float) wi * data[k2 + 1];
                        tempi = (float) wr * data[k2 + 1]+(float) wi * data[k2];
                        data[k2] = data[k1] - tempr;
                        data[k2 + 1] = data[k1 + 1] - tempi;
                        data[k1] += tempr;
                        data[k1 + 1] += tempi;
                    }
                }
                wr = (wtemp = wr) * wpr - wi * wpi + wr;
                wi = wi * wpr + wtemp * wpi + wi;
            }
            ifp1 = ifp2;
        }
        nprev *= n;
    }
}

#undef SWAP
