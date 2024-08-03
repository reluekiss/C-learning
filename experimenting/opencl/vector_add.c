#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#define VECTOR_SIZE 1024
int main() {
    // Initialize OpenCL
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_command_queue queue = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;

    // Get platform and device
    cl_uint num_platforms;
    clGetPlatformIDs(1, &platform_id, &num_platforms);
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

    // Create context and command queue
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, NULL);

    // Create program and kernel
    const char *kernel_source = "__kernel void add_vectors(__global int *a, __global int *b, __global int *c) { int gid = get_global_id(0); c[gid] = a[gid] + b[gid]; }";
    program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, NULL);
    clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "add_vectors", NULL);

    // Create buffers for input and output vectors
    int *a = (int *)malloc(VECTOR_SIZE * sizeof(int));
    int *b = (int *)malloc(VECTOR_SIZE * sizeof(int));
    int *c = (int *)malloc(VECTOR_SIZE * sizeof(int));

    // Initialize input vectors
    for (int i = 0; i < VECTOR_SIZE; i++) {
        a[i] = i;
        b[i] = i * 2;
    }

    // Create OpenCL buffers
    cl_mem a_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, VECTOR_SIZE * sizeof(int), NULL, NULL);
    cl_mem b_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, VECTOR_SIZE * sizeof(int), NULL, NULL);
    cl_mem c_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, VECTOR_SIZE * sizeof(int), NULL, NULL);

    // Copy input vectors to OpenCL buffers
    clEnqueueWriteBuffer(queue, a_buffer, CL_TRUE, 0, VECTOR_SIZE * sizeof(int), a, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, b_buffer, CL_TRUE, 0, VECTOR_SIZE * sizeof(int), b, 0, NULL, NULL);

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &a_buffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &b_buffer);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &c_buffer);

    // Execute kernel
    size_t global_work_size = VECTOR_SIZE;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);

    // Read output vector from OpenCL buffer
    clEnqueueReadBuffer(queue, c_buffer, CL_TRUE, 0, VECTOR_SIZE * sizeof(int), c, 0, NULL, NULL);

    // Print result
    for (int i = 0; i < VECTOR_SIZE; i++) {
        printf("c[%d] = %d\n", i, c[i]);
    }

    // Clean up
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(a);
    free(b);
    free(c);

    return 0;
}
