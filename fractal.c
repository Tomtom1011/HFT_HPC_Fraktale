#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main() {
    // Allocate target array for grayscale image
    // unsigned char *gray = malloc(width * height);

    printf("Hallo");

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
    // stbi_write_jpg("grayscale.jpg", width, height, 1, gray, 95);
}