#include "OpenCLWrapper.h"
#include <fstream>
#include <iostream>
#include <Exception.h>

OpenCLWrapper::OpenCLWrapper() {
}

OpenCLWrapper::~OpenCLWrapper() {
}

void OpenCLWrapper::init_platform() {
    CALL_CL_GUARDED(clGetPlatformIDs, (1, &m_platform, NULL));
}

void OpenCLWrapper::init_device(bool use_cpu) {
    cl_device_type device_type =
        use_cpu ? CL_DEVICE_TYPE_CPU : CL_DEVICE_TYPE_GPU;
    CALL_CL_GUARDED(clGetDeviceIDs, (m_platform, device_type, 1, &m_device, NULL));
}

void OpenCLWrapper::init_context() {
    cl_int err;
    m_context = clCreateContext(NULL, 1, &m_device, NULL, NULL, &err);
    CHECK_CL_ERROR(err, "clCreateContext");
}

void OpenCLWrapper::init_queue() {
    cl_int err;
    cl_command_queue_properties qprops = 0;
    m_queue = clCreateCommandQueue(m_context, m_device, qprops, &err);
    CHECK_CL_ERROR(err, "clCreateCommandQueue");
}

void OpenCLWrapper::init_program(const std::string& prog) {
    cl_int err;
    const char* raw_contents = prog.c_str();
    m_program = clCreateProgramWithSource(
            m_context,
            1,
            &raw_contents,
            NULL, 
            &err);
    CHECK_CL_ERROR(err, "clCreateProgramWithSource");
}

void OpenCLWrapper::build_program() {
    //const char compiler_flags[] = "-cl-finite-math-only";
    const char compiler_flags[] = "";
    cl_int err;
    err = clBuildProgram(
            m_program,
            1,
            &m_device,
            compiler_flags,
            NULL,
            NULL);
    if (err != 0) {
        std::cout << "OpenCL build failed..." << std::endl;
        char buffer[1024];
        clGetProgramBuildInfo(
                m_program,
                m_device,
                CL_PROGRAM_BUILD_LOG,
                1024,
                buffer,
                NULL);
        std::cout << buffer << std::endl;
    }
}

void OpenCLWrapper::init_kernel(const std::string& kernel_name) {
    cl_int err;
    m_kernel = clCreateKernel(m_program, kernel_name.c_str(), &err);
    CHECK_CL_ERROR(err, "clCreateKernel");
}

cl_mem OpenCLWrapper::create_buffer(size_t num_bytes, void* raw_data) {
    cl_mem buf = allocate_memory_on_device(num_bytes);
    if (raw_data != NULL)
        write_to_buffer(buf, 0, num_bytes, raw_data);
    return buf;
}

void OpenCLWrapper::read_from_buffer(
        cl_mem buf, size_t offset, size_t num_bytes, void* data) {
    CALL_CL_GUARDED(clEnqueueReadBuffer,
            (m_queue, buf, CL_TRUE,
             offset, num_bytes, data,
             0, NULL, NULL));
}

void OpenCLWrapper::write_to_buffer(
        cl_mem buf, size_t offset, size_t num_bytes, void* data) {
    CALL_CL_GUARDED(clEnqueueWriteBuffer,
            (m_queue, buf, CL_TRUE,
             offset, num_bytes, data,
             0, NULL, NULL));
}

size_t OpenCLWrapper::get_kernel_work_group_size() const {
    size_t work_group_size;
    CALL_CL_GUARDED(clGetKernelWorkGroupInfo, (
                m_kernel, m_device,
                CL_KERNEL_WORK_GROUP_SIZE,
                sizeof(size_t), &work_group_size,
                NULL));
    return work_group_size;
}

size_t OpenCLWrapper::get_num_work_groups(
        size_t total_num_work_items) const {
    size_t work_group_size = get_kernel_work_group_size();
    size_t num_workgroups = total_num_work_items / work_group_size;
    if (total_num_work_items % work_group_size > 0)
        num_workgroups ++;
    return num_workgroups;
}

void OpenCLWrapper::execute_kernel(size_t num_work_items) {
    size_t work_group_size = get_kernel_work_group_size();
    CALL_CL_GUARDED(clEnqueueNDRangeKernel, (
                m_queue, m_kernel,
                1, NULL, &num_work_items, &work_group_size,
                0, NULL, NULL));
    CALL_CL_GUARDED(clFinish, (m_queue));
}

cl_mem OpenCLWrapper::allocate_memory_on_device(size_t num_bytes) const {
    cl_int err;
    cl_mem_flags flags = CL_MEM_READ_WRITE;

    cl_mem buf = clCreateBuffer(
            m_context,
            flags,
            num_bytes,
            NULL,
            &err);
    CHECK_CL_ERROR(err, "clCreateBuffer");
    return buf;
}

std::string OpenCLWrapper::read_file(const std::string& filename) {
    std::ifstream fin(filename.c_str());
    if (!fin) {
        throw RuntimeError(
                std::string("Unable to open program file: ") +
                filename);
    }
    std::string content(
            (std::istreambuf_iterator<char>(fin) ),
            (std::istreambuf_iterator<char>()    ) );
    return content;
}


