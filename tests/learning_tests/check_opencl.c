#include "cl-helper.h"

void print_platform_devices(cl_platform_id platform) {
    cl_uint num_devices;
    CALL_CL_GUARDED(clGetDeviceIDs, (platform, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices));
    cl_device_id* devices = malloc(sizeof(cl_device_id) * num_devices);
    CALL_CL_GUARDED(clGetDeviceIDs, (platform, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL));

    for (size_t i=0; i<num_devices; i++) {
        char name[128];
        char vendor[128];
        cl_ulong glob_mem_size;
        cl_ulong const_mem_size;
        cl_ulong local_mem_size;
        cl_ulong cache_mem_size;
        cl_uint vector_width_char;
        cl_uint vector_width_short;
        cl_uint vector_width_int;
        cl_uint vector_width_long;
        cl_uint vector_width_float;
        cl_uint vector_width_double;
        size_t max_work_group_size;

        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_NAME, sizeof(name), name, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_VENDOR, sizeof(vendor), vendor, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_GLOBAL_MEM_SIZE,
                    sizeof(cl_ulong), &glob_mem_size, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,
                    sizeof(cl_ulong), &const_mem_size, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_LOCAL_MEM_SIZE,
                    sizeof(cl_ulong), &local_mem_size, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,
                    sizeof(cl_ulong), &cache_mem_size, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR,
                    sizeof(cl_uint), &vector_width_char, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,
                    sizeof(cl_uint), &vector_width_short, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,
                    sizeof(cl_uint), &vector_width_int, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG,
                    sizeof(cl_uint), &vector_width_long, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,
                    sizeof(cl_uint), &vector_width_float, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE,
                    sizeof(cl_uint), &vector_width_double, NULL));
        CALL_CL_GUARDED(clGetDeviceInfo, (devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE,
                    sizeof(size_t), &max_work_group_size, NULL));

        printf("--------------------------------------------------------------------------------\n");
        printf("device name: %s\n", name);
        printf("device vendor: %s\n", vendor);
        printf("device glob  mem size: %llu\n", glob_mem_size);
        printf("device const mem size: %llu\n", const_mem_size);
        printf("device local mem size: %llu\n", local_mem_size);
        printf("device cache mem size: %llu\n", cache_mem_size);
        printf("device preferred vec width: %u (char)\n", vector_width_char);
        printf("device preferred vec width: %u (short)\n", vector_width_short);
        printf("device preferred vec width: %u (int)\n", vector_width_int);
        printf("device preferred vec width: %u (long)\n", vector_width_long);
        printf("device preferred vec width: %u (float)\n", vector_width_float);
        printf("device preferred vec width: %u (double)\n", vector_width_double);
        printf("device max work group size: %zu\n", max_work_group_size);
    }
    free(devices);
}

void print_info() {
    cl_uint num_platforms;

    CALL_CL_GUARDED(clGetPlatformIDs, (1, NULL, &num_platforms));
    cl_platform_id* platforms = malloc(sizeof(cl_platform_id)*num_platforms);
    CALL_CL_GUARDED(clGetPlatformIDs, (num_platforms, platforms, NULL));

    for (size_t i=0; i<num_platforms; i++) {
        char name[40];
        char vendor[40];
        char version[40];
        CALL_CL_GUARDED(clGetPlatformInfo, (platforms[i], CL_PLATFORM_NAME, 40, name, NULL));
        CALL_CL_GUARDED(clGetPlatformInfo, (platforms[i], CL_PLATFORM_VENDOR, 40, vendor, NULL));
        CALL_CL_GUARDED(clGetPlatformInfo, (platforms[i], CL_PLATFORM_VERSION, 40, version, NULL));
        printf("platform name: %s\n", name);
        printf("platform vendor: %s\n", vendor);
        printf("platform version: %s\n", version);
        print_platform_devices(platforms[i]);
    }

    free(platforms);
}

int main() {
    print_info();
    //print_platforms_devices();
    //cl_context ctx;
    //cl_command_queue queue;
    //create_context_on(CHOOSE_INTERACTIVELY, CHOOSE_INTERACTIVELY, 0, &ctx, &queue, 0);
}
