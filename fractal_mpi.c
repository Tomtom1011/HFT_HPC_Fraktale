#include <mpi.h>
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
int MAX_ITER = 10000;

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

const int ROOT_NODE = 0;

int main() {
    printf("Main");

    // MPI Init
    MPI_Init(NULL, NULL);
    int numberOfNodes;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfNodes);
    int nodeRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &nodeRank);

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
    unsigned char *allFractals;

    int *receivedSizes;
    int *offsets;

    printf("Start\n");
    
    if (nodeRank == ROOT_NODE) {
        startTimeMeasure();

        allFractals = malloc(aufloesungX * aufloesungY * channel * sizeof(unsigned char));
        receivedSizes = malloc(numberOfNodes * sizeof(int));
        offsets = malloc(numberOfNodes * sizeof(int));
    }

    int numberOfYCalculationsForNode = aufloesungY / numberOfNodes;
    int yCoordinateOffset = nodeRank * numberOfYCalculationsForNode;
    if (nodeRank == (numberOfNodes - 1)) {
        numberOfYCalculationsForNode += (aufloesungY - ((nodeRank + 1) * numberOfYCalculationsForNode));
    }

    int sizeForNode = numberOfYCalculationsForNode * aufloesungX * channel * sizeof(unsigned char);
    MPI_Gather(&sizeForNode, 1, MPI_INT, receivedSizes, 1, MPI_INT, ROOT_NODE, MPI_COMM_WORLD);
    
    if (nodeRank == ROOT_NODE) {
        offsets[0] = 0;
        for (int i = 0; i < numberOfNodes - 1; i++) {
            offsets[i+1] = offsets[i] + receivedSizes[i];
        }
    }

   printf("Vor Berechnung - Rank=%i\n", nodeRank);

    /*if (nodeRank == ROOT_NODE) {
        for (int i = 0; i < numberOfNodes; i++) {
            printf("0 - Rank=%i offset[%i]=%i receivedSizes[%i]=%i\n", nodeRank, i, offsets[i], i, receivedSizes[i]);
        }
    }*/

    unsigned char *localFractal = malloc(numberOfYCalculationsForNode * aufloesungX * channel * sizeof(unsigned char));

    for (int indexY = 0; indexY < numberOfYCalculationsForNode; indexY++) {
        for (int indexX = 0; indexX < aufloesungX; indexX++) {

            double xCoord = wbXStart + (indexX * xStep);
            double yCoord = wbYStart - ((indexY + yCoordinateOffset) * yStep);

            int iter = iterate(MAX_ITER, xCoord, yCoord);

            localFractal[(indexY * aufloesungX + indexX) * channel + 0] = (char) iter;
            localFractal[(indexY * aufloesungX + indexX) * channel + 1] = (char) iter;
            localFractal[(indexY * aufloesungX + indexX) * channel + 2] = (char) iter;
        }
    }
    
    /*
    printf("1 - Rank=%i sendbuf=%i sendcountPart=%i receivedcountAll=%i\n", nodeRank, localFractal[0], (numberOfYCalculationsForNode * aufloesungX * channel), (aufloesungX * aufloesungY * channel));
    if (nodeRank == ROOT_NODE) {
        printf("2 - Rank=%i recvbuf[%i]=%i\n", nodeRank, 0, allFractals[0]);
        for (int i = 0; i < numberOfNodes; i++) {
            printf("3 - Rank=%i recvcounts[%i]=%i displs[%i]=%i\n", nodeRank, i, receivedSizes[i], i, offsets[i]);
        }
    }
    printf("4 - Rank=%i localFractalCount=%li\n", nodeRank, sizeof(localFractal)/sizeof(unsigned char));
    */

    printf("Vor Gather - Rank=%i\n", nodeRank);

    // Die Daten zurück senden
    MPI_Gatherv(localFractal,   // void *sendbuf
            (numberOfYCalculationsForNode * aufloesungX * channel), // int sendcount
            MPI_UNSIGNED_CHAR,  // MPI_Datatype sendtype
            allFractals,        // void *recvbuf
            receivedSizes,      // int *recvcounts
            offsets,            // int *displs
            MPI_UNSIGNED_CHAR,  // MPI_Datatype recvtype
            ROOT_NODE,          // int root
            MPI_COMM_WORLD);    // MPI_Comm comm
    
    free(localFractal);
    MPI_Finalize();

    if (nodeRank == ROOT_NODE) {
        printf("7 - Rank=%i AllFraktal[0]=%i AllFraktal[135]=%i AllFraktal[270]=%i\n", 
                nodeRank, allFractals[0], allFractals[135], allFractals[270]);

        stopTimeMeasure();

        writeToFile(aufloesungX, aufloesungY, channel, allFractals);
        free(receivedSizes);
        free(offsets);
        free(allFractals);
    }
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
    char filename[25];
    sprintf(filename, "fractal_mpi_%i.jpg", MAX_ITER);
    stbi_write_jpg(filename, aufloesungX, aufloesungY, channel, fractal, 95);
    printf("Datei geschrieben\n");
}