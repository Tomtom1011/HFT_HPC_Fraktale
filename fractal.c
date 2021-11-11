#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct complexDouble {
    double real;
    double imag;
};

struct complexDouble iterate(int anzahlIter, int x, int y);

int main() {
    // Allocate target array for grayscale image
    /*
    int width = 800;
    int height = 600;
    int testchannel = 3;
    unsigned char *gray = malloc(width * height);
    unsigned char *bunt = malloc(width * height * testchannel);
    */

    // Test Grau Bild

    /*
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            gray[y * width + x] = rand();
        }
    }
    stbi_write_jpg("grayscale.jpg", width, height, 1, gray, 95);

    free(gray);
    */

    // Test Buntes Bild

    /*
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            bunt[(y * width + x) * 3 + 0] = rand();
            bunt[(y * width + x) * 3 + 1] = rand();
            bunt[(y * width + x) * 3 + 2] = rand();
        }
    }
    stbi_write_jpg("bunt.jpg", width, height, 3, bunt, 95);

    free(bunt);
    */

    // Test mit Komplexen Zahlen
    /*
    double complex z1 = 1.0 + 3.0 * I;
    double complex z2 = 1.0 - 4.0 * I;

    double complex sum = z1 / z2;

    printf("%f %f", creal(sum), cimag(sum));
    */
    // Start Fraktale

    double wbXStart = -2;
    double wbXEnd = 1;

    double wbYStart = -1;
    double wbYEnd = 1;

    int aufloesungX = 1920;
    int aufloesungY = 1080;
    int channel = 3;

    double xStep = (wbXEnd - wbXStart) / aufloesungX;
    double yStep = (wbYEnd - wbYStart) / aufloesungY;

    unsigned char *fractal = malloc(aufloesungX * aufloesungY * channel);

    for (int x = wbXStart; x < wbXEnd; x += xStep) {
        for (int y = wbYStart; y < wbYEnd; y += yStep) {
            struct complexDouble z = iterate(1, x, y);
            double complex zn = z.real + z.imag * I;
            fractal[(y * aufloesungX + x) * 3 + 0] = creal(zn) + cimag(zn);
            fractal[(y * aufloesungX + x) * 3 + 1] = creal(zn) + cimag(zn);
            fractal[(y * aufloesungX + x) * 3 + 2] = creal(zn) + cimag(zn);
        }
    }

    printf("Hallo");
    stbi_write_jpg("fractal.jpg", aufloesungX, aufloesungY, channel, fractal,
                   95);
}

struct complexDouble iterate(int anzahlIter, int x, int y) {
    // zn0 = 0
    // zn+1 = zn² + c
    double complex zn = 0 + 0 * I;
    for (int i = 0; i < anzahlIter; i++) {
        zn = (zn * zn) + (x + y * I);
    }
    struct complexDouble z;
    z.real = creal(zn);
    z.imag = cimag(zn);
    return z;
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