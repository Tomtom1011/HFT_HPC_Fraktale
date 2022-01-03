#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <math.h>
#include <sys/time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int iterate(int maxItert, double x, double y);
void startTimeMeasure();
void stopTimeMeasure();
void writeToFile(int aufloesungX, int aufloesungY, int channel, unsigned char* fractal);

/**
 * Die Anzahl an Maximalen Iterationen zur Mandelbrot Berechnung
 */
int MAX_ITER = 100;

/**
 * Zeitmessung Start
 */
struct timeval start;

/**
 * Standard = -2.5
 */
double wbXStart = -2.5;

/**
 * Standard 1.5
 */
double wbXEnd = 1.5;

/**
 * Standard 1.5
 */
double wbYStart = 1.5;

/**
 * Standard -1.5
 */
double wbYEnd = -1.5;

/**
 * Bildaufloesung in X Richtung
 */
int aufloesungX = 1920;

/**
 * Bildaufloesung in Y Richtung
 */
int aufloesungY = 1080;

/**
 * Kanalanzahl der Datei
 */
int channel = 3;

int main() {
    
    /**
     * Berechnete Schrittgroeße zwischen den Pixel zur Aufloesung in X Richtung
     */
    double xStep = (wbXEnd - wbXStart) / aufloesungX;

    /**
     * Berechnete Schrittgroeße zwischen den Pixel zur Aufloesung in Y Richtung
     */
    double yStep = (wbYStart - wbYEnd) / aufloesungY;

    /**
     * Das Datenarray fuer das Fraktal
     */
    unsigned char *fractal = malloc((aufloesungX+1) * (aufloesungY+1) * channel);

    startTimeMeasure();

    for (int indexY = 0; indexY < aufloesungY; indexY++) {
        for (int indexX = 0; indexX < aufloesungX; indexX++) {

            double xCoord = wbXStart + (indexX * xStep);
            double yCoord = wbYStart - (indexY * yStep);

            int iter = iterate(MAX_ITER, xCoord, yCoord);

            fractal[(indexY * aufloesungX + indexX) * channel + 0] = (char) iter;
            fractal[(indexY * aufloesungX + indexX) * channel + 1] = (char) iter;
            fractal[(indexY * aufloesungX + indexX) * channel + 2] = (char) iter;
        }
    }

    stopTimeMeasure();

    writeToFile(aufloesungX, aufloesungY, channel, fractal);

}

int iterate(int maxItert, double x, double y) {
    // zn0 = 0
    // zn+1 = zn² + c
    double complex zn = x + y * I;

    int iter = 1;
    double betrag = 1;
    do {

        double complex pow = (zn * zn);
        double complex add = pow + (x + y * I);

        zn = add;
        const double real = creal(zn) * creal(zn);
        const double imag = cimag(zn) * cimag(zn);
        betrag = sqrt( real + imag );
        iter++;
    } while (iter <= maxItert && betrag <= 2);

    return iter;
}

void startTimeMeasure() {
    gettimeofday(&start, 0);
}

void stopTimeMeasure() {
    struct timeval end;
    gettimeofday(&end, 0);
    long lsec = end.tv_sec - start.tv_sec;
    long lusec = end.tv_usec - start.tv_usec;
    double sec = (lsec + lusec / 1000000.0);
    printf("%8.6f seconds\n", sec);
}

void writeToFile(int aufloesungX, int aufloesungY, int channel, unsigned char* fractal) {
    char filename[20];
    sprintf(filename, "fractal_seq_%i.jpg", MAX_ITER);
    stbi_write_jpg(filename, aufloesungX, aufloesungY, channel, fractal, 95);
    printf("Datei geschrieben\n");
}