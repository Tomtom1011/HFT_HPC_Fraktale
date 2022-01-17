#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define CL_PROGRAM_FILE "opencl-program.cl"

#define MAX_PLATFORMS 10
#define MAX_DEVICES 10
#define MAX_NAME_LENGTH 128

void writeToFile(int aufloesungX, int aufloesungY, int channel, unsigned char* fractal, int MAX_ITER);

int main(int argc, char** argv)
{
    // First get available platforms
    cl_platform_id* platforms;
    cl_uint n_platforms;
    clGetPlatformIDs(MAX_PLATFORMS, NULL, &n_platforms);
    platforms = (cl_platform_id*)malloc(n_platforms * sizeof(cl_platform_id));
    clGetPlatformIDs(n_platforms, platforms, NULL);

    if (n_platforms == 0) {
        fprintf(stderr, "No platforms found. Exiting.");
        return -1;
    }

    char name[MAX_NAME_LENGTH];
    printf("Platforms:\n");
    for (int i = 0; i < n_platforms; i++)
    {
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(name), name, NULL);
        printf("[%d]: %s\n", i, name);
    }
    printf("Choose platform (Default 0): ");
    int choice = 0;
    scanf_s("%d", &choice);
    if (choice < 0 || choice >= n_platforms) {
        printf("Invalid choice, using platform 0");
        choice = 0;
    }
    cl_platform_id platform;
    memcpy(&platform, &platforms[choice], sizeof(cl_platform_id));
    free(platforms);

    // Get Devices for chosen platform
    cl_device_id* devices;
    cl_uint n_devices;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, MAX_DEVICES, NULL, &n_devices);
    devices = (cl_device_id*)malloc(n_devices * sizeof(cl_device_id));
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, n_devices, devices, NULL);
    if (n_devices == 0) {
        fprintf(stderr, "No devices found for platform. Exiting.\n");
        return -1;
    }

    printf("Devices:\n");
    for (int i = 0; i < n_devices; i++) {
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(name), name, NULL);
        printf("[%d]: %s\n", i, name);
    }
    printf("Choose device (Default 0): ");
    choice = 0;
    scanf_s("%d", &choice);
    if (choice < 0 || choice >= n_devices) {
        printf("Invalid choice, using device 0");
        choice = 0;
    }
    cl_device_id device;
    memcpy(&device, &devices[choice], sizeof(cl_device_id));
    free(devices);

    // Create a compute context 
    cl_context context;
    context = clCreateContext(0, 1, &device, NULL, NULL, NULL);
    if (!context)
    {
        fprintf(stderr, "Failed to create a compute context.\n");
        return -1;
    }

    // Create a command queue
    cl_command_queue commands = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, NULL);
    if (!commands)
    {
        fprintf(stderr, "Failed to create a command queue.\n");
        return -1;
    }


    // Load program from file
    FILE* program_file = fopen(CL_PROGRAM_FILE, "rb");
    if (program_file == NULL) {
        fprintf(stderr, "Failed to open OpenCL program file\n");
        return -1;
    }
    fseek(program_file, 0, SEEK_END);
    size_t program_size = ftell(program_file);
    rewind(program_file);
    char* program_text = (char*)malloc((program_size + 1) * sizeof(char));
    program_text[program_size] = '\0';
    fread(program_text, sizeof(char), program_size, program_file);
    fclose(program_file);

    int err;
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&program_text, NULL, &err);
    if (err < 0)
    {
        fprintf(stderr, "Failed to create program.\n");
        return -1;
    }

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        fprintf(stderr, "Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        fprintf(stderr, "%s\n", buffer);
        return -1;
    }
    free(program_text);

    // Create Kernel ... has to be adapted to program
    cl_kernel vector_add = clCreateKernel(program, "calculateMandelBrotStep", &err);
    if (!vector_add || err != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to create compute kernel!\n");
        return -1;
    }






    int MAX_ITER = 100;
    double wbXStart = -2.5;
    double wbXEnd = 1.5;
    double wbYStart = 1.5;
    double wbYEnd = -1.5;
    long long aufloesungX = 3840; // 3840 x 2160 
    long long aufloesungY = 2160;
    long long channel = 3;

    double xStep = (wbXEnd - wbXStart) / aufloesungX;
    double yStep = (wbYStart - wbYEnd) / aufloesungY;

    const long long FRACTAL_ARRAY_SIZE = aufloesungX * aufloesungY * channel * sizeof(unsigned char);

    // Daten auf dem Host vorbereiten
    unsigned char* fractal = (unsigned char*) malloc(FRACTAL_ARRAY_SIZE);

    // Create memory for parameters in device
    cl_mem gpuMem_MAX_ITER = clCreateBuffer(context, CL_MEM_READ_ONLY, 1 * sizeof(int), NULL, NULL);
    cl_mem gpuMem_wbXStart = clCreateBuffer(context, CL_MEM_READ_ONLY, 1 * sizeof(double), NULL, NULL);
    cl_mem gpuMem_wbYStart = clCreateBuffer(context, CL_MEM_READ_ONLY, 1 * sizeof(double), NULL, NULL);
    cl_mem gpuMem_xStep = clCreateBuffer(context, CL_MEM_READ_ONLY, 1 * sizeof(double), NULL, NULL);
    cl_mem gpuMem_yStep = clCreateBuffer(context, CL_MEM_READ_ONLY, 1 * sizeof(double), NULL, NULL);
    cl_mem gpuMem_aufloesungX = clCreateBuffer(context, CL_MEM_READ_ONLY, 1 * sizeof(int), NULL, NULL);
    cl_mem gpuMem_channel = clCreateBuffer(context, CL_MEM_READ_ONLY, 1 * sizeof(int), NULL, NULL);
    cl_mem gpuMem_fractal = clCreateBuffer(context, CL_MEM_WRITE_ONLY, FRACTAL_ARRAY_SIZE, NULL, NULL);
    
    if (!gpuMem_MAX_ITER || !gpuMem_wbXStart || !gpuMem_wbYStart || !gpuMem_xStep || 
        !gpuMem_yStep || !aufloesungX || ! channel || !gpuMem_fractal)
    {
        fprintf(stderr, "Failed to allocate memory on device\n");
        return -1;
    }

    // Copy data of parameters to device
    err = clEnqueueWriteBuffer(commands, gpuMem_MAX_ITER, CL_TRUE, 0, 1 * sizeof(int), &MAX_ITER, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to write to device memory MAX_ITER \n");
        return -1;
    }

    clFinish(commands);

    err = clEnqueueWriteBuffer(commands, gpuMem_wbXStart, CL_TRUE, 0, 1 * sizeof(double), &wbXStart, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to write to device memory wbXStart\n");
        return -1;
    }

    clFinish(commands);

    err = clEnqueueWriteBuffer(commands, gpuMem_wbYStart, CL_TRUE, 0, 1 * sizeof(double), &wbYStart, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to write to device memory wbYStart\n");
        return -1;
    }

    clFinish(commands);

    err = clEnqueueWriteBuffer(commands, gpuMem_xStep, CL_TRUE, 0, 1 * sizeof(double), &xStep, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to write to device memory xStep\n");
        return -1;
    }

    clFinish(commands);

    err = clEnqueueWriteBuffer(commands, gpuMem_yStep, CL_TRUE, 0, 1 * sizeof(double), &yStep, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to write to device memory yStep\n");
        return -1;
    }

    clFinish(commands);

    err = clEnqueueWriteBuffer(commands, gpuMem_aufloesungX, CL_TRUE, 0, 1 * sizeof(int), &aufloesungX, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to write to device memory aufloesungX\n");
        return -1;
    }

    clFinish(commands);

    err = clEnqueueWriteBuffer(commands, gpuMem_channel, CL_TRUE, 0, 1 * sizeof(int), &channel, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to write to device memory channel\n");
        return -1;
    }

    err = clFinish(commands);


    /*
        __kernel void calculateMandelBrotStep(__global const int MAX_ITER,
        __global const double wbXStart,
        __global const double wbYStart,
        __global const double xStep,
        __global const double yStep,
        __global const int aufloesungX,
        __global const int channel,
        __global unsigned char* fractal)
    */

    // Set the arguments to our compute kernel
    err = 0;
    err = clSetKernelArg(vector_add, 0, sizeof(cl_mem), &gpuMem_MAX_ITER);
    err |= clSetKernelArg(vector_add, 1, sizeof(cl_mem), &gpuMem_wbXStart);
    err |= clSetKernelArg(vector_add, 2, sizeof(cl_mem), &gpuMem_wbYStart);
    err |= clSetKernelArg(vector_add, 3, sizeof(cl_mem), &gpuMem_xStep);
    err |= clSetKernelArg(vector_add, 4, sizeof(cl_mem), &gpuMem_yStep);
    err |= clSetKernelArg(vector_add, 5, sizeof(cl_mem), &gpuMem_aufloesungX);
    err |= clSetKernelArg(vector_add, 6, sizeof(cl_mem), &gpuMem_channel);
    err |= clSetKernelArg(vector_add, 7, sizeof(cl_mem), &gpuMem_fractal);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to set kernel arguments!\n");
        return -1;
    }

    // Time measurements
    cl_event prof_event;
    cl_ulong start_time, end_time;
    size_t return_bytes;

    // Execute the kernel               
    size_t global_size[] = {aufloesungX, aufloesungY};
    err = clEnqueueNDRangeKernel(commands, vector_add, 2, NULL, global_size, NULL, 0, NULL, &prof_event);
    if (err)
    {
        fprintf(stderr, "Failed to execute kernel!\n");
        return -1;
    }

    // Wait for the commands to get serviced before reading back results
    err = clFinish(commands);
    printf("clFinish Command ausführen errCode=%i", err);

    clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start_time, &return_bytes);
    clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end_time, &return_bytes);
    double time = (double)(end_time - start_time) / 1.0e9;
    printf("start=%f  end=%f\n", (double) start_time, (double) end_time);
    printf("Zeit: %.4f Sec (nur Berechnung ohne Memory-Transfer)\nLeistung %.2f GFLOP/s\n", time, (FRACTAL_ARRAY_SIZE * (4 + MAX_ITER * 11)) / time / 1e9);

    // Read back the results from the device to verify the output
    err = clEnqueueReadBuffer(commands, gpuMem_fractal, CL_TRUE, 0, FRACTAL_ARRAY_SIZE, fractal, 0, NULL, NULL);    // TODO Fehler zwischen global_size 200000 - 240000 (500x400 funktioniert noch)
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to read output array");
        printf("Fehlercode %i", err);
        return -1;
    }

    writeToFile(aufloesungX, aufloesungY, channel, fractal, MAX_ITER);

    // Shutdown and cleanup
    clReleaseMemObject(gpuMem_MAX_ITER);
    clReleaseMemObject(gpuMem_wbXStart);
    clReleaseMemObject(gpuMem_wbYStart);
    clReleaseMemObject(gpuMem_xStep);
    clReleaseMemObject(gpuMem_yStep);
    clReleaseMemObject(gpuMem_aufloesungX);
    clReleaseMemObject(gpuMem_channel);
    clReleaseMemObject(gpuMem_fractal);

    clReleaseProgram(program);
    clReleaseKernel(vector_add);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);

    free(fractal);

    return 0;
}

void writeToFile(int aufloesungX, int aufloesungY, int channel, unsigned char* fractal, int MAX_ITER) {
    char filename[25];
    sprintf(filename, "fractal_gpu_%i_%i_%i.jpg", MAX_ITER, aufloesungX, aufloesungY);
    stbi_write_jpg(filename, aufloesungX, aufloesungY, channel, fractal, 95);
    printf("Datei geschrieben\n");
}