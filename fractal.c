#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int iterate(int maxItert, double x, double y);

void groundColorMix(double* color, int x, int min, int max);

int color_rotate = 0;

// Start Fraktale
int MAX_ITER = 100;

int main() {
    
    double wbXStart = -2.5;
    double wbXEnd = 1.5;

    double wbYStart = 1.5;
    double wbYEnd = -1.5;

    int aufloesungX = 1920;
    int aufloesungY = 1080;
    int channel = 3;

    double xStep = (wbXEnd - wbXStart) / aufloesungX;
    double yStep = (wbYStart - wbYEnd) / aufloesungY;

    printf ("Step X=%f : Y=%f\n", xStep, yStep);

    unsigned char *fractal = malloc((aufloesungX+1) * (aufloesungY+1) * channel);

    int minIter = MAX_ITER, maxIter = 0;

    for (int indexY = 0; indexY < aufloesungY; indexY++) {
        for (int indexX = 0; indexX < aufloesungX; indexX++) {

            double xCoord = wbXStart + (indexX * xStep);
            double yCoord = wbYStart - (indexY * yStep);

            int iter = iterate(MAX_ITER, xCoord, yCoord);

            
            if (iter > maxIter) {
                maxIter = iter;
            } 
            if (iter < minIter) {
                minIter = iter;
            }
            

            double arr[3];

            groundColorMix(arr, iter, minIter, maxIter);
            
            fractal[(indexY * aufloesungX + indexX) * channel + 0] = (char) arr[0]; //iter;
            fractal[(indexY * aufloesungX + indexX) * channel + 1] = (char) arr[1]; //iter;
            fractal[(indexY * aufloesungX + indexX) * channel + 2] = (char) arr[2]; //iter;
            
        }
    }

    printf("Ende %i\n", MAX_ITER);

    char filename[20];

    sprintf(filename, "fractal_%i.jpg", MAX_ITER);

    stbi_write_jpg(filename, aufloesungX, aufloesungY, channel, fractal,
                   95);
}

int iterate(int maxItert, double x, double y) {
    // zn0 = 0
    // zn+1 = zn² + c
    double complex zn = x + y * I;

    int iter = 1;
    double betrag = 1;
    do {

        //printf("Durchgang %i -> %f : %f\n", iter, creal(zn), cimag(zn));

        double complex pow = (zn * zn);
        //printf("Pow %f : %f\n", creal(pow), cimag(pow));

        double complex add = pow + (x + y * I);
        //printf("Add %f : %f\n", creal(add), cimag(add));

        zn = add;
        const double real = creal(zn) * creal(zn);
        const double imag = cimag(zn) * cimag(zn);
        betrag = sqrt( real + imag );
        iter++;

    } while (iter <= maxItert && betrag <= 2);

    return iter;
}

// TODO Zeitmessen Start
// struct timeval start;
// gettimeofday(&start, 0);

// TODO Zeitmessen Ende
/*
struct timeval end;
gettimeofday(&end, 0);
long lsec = end.tv_sec - start.tv_sec;
long lusec = end.tv_usec - start.tv_usec;
double sec = (lsec + lusec / 1000000.0);
printf("%8.6f seconds\n", sec);
*/

// TODO FLOP Berechnung
// double flop = width * height * (3 + 2);
// printf("%8.2f MFLOP\n", flop / 1000000.0);

// TODO Ausgabe FLOP/s
// double mflops = flop / 1000000.0 / sec;
// printf("%8.2f MFLOP/s\n", mflops);

// Bild schreiben
// stbi_write_jpg("grayscale.jpg", width, height, 3, gray, 95);