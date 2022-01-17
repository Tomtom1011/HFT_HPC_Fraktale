typedef float2 cfloat;

#define I ((cfloat)(0.0, 1.0))

inline float  real(cfloat a) {
    return a.x;
}
inline float  imag(cfloat a) {
    return a.y;
}

inline cfloat  cmult(cfloat a, cfloat b) {
    return (cfloat)(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

inline cfloat  cadd(cfloat a, cfloat b) {
    return (cfloat)(a.x + b.x, a.y + b.y);
}

inline float cmod(cfloat a) {
    return (sqrt(a.x * a.x + a.y * a.y));
}

__kernel void calculateMandelBrotStep(__global const int *MAX_ITER,
        __global const double *wbXStart,
        __global const double *wbYStart,
        __global const double *xStep,
        __global const double *yStep,
        __global const int *aufloesungX,
        __global const int *channel,
        __global unsigned char *fractal)
{
    int indexX = get_global_id(0);
    int indexY = get_global_id(1);
	
    double xCoord = *wbXStart + (indexX * *xStep);
    double yCoord = *wbYStart - (indexY * *yStep);

    cfloat coord;
    coord.x = xCoord;
    coord.y = yCoord;

    cfloat zn;
    zn.x = xCoord;
    zn.y = yCoord;

    int iter = 1;
    double betrag = 1;
    do {
        cfloat pow = cmult(zn, zn);
        cfloat add = cadd(pow, coord);

        zn = add;
        betrag = cmod(zn);
        iter++;
    } while (iter <= *MAX_ITER && betrag <= 2);
	
    fractal[(indexY * *aufloesungX + indexX) * *channel + 0] = (char) iter;
    fractal[(indexY * *aufloesungX + indexX) * *channel + 1] = (char) iter;
    fractal[(indexY * *aufloesungX + indexX) * *channel + 2] = (char) iter;

}