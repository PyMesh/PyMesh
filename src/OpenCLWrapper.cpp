#include "OpenCLWrapper.h"
#include <fstream>
#include <iostream>

#include "Exception.h"
#include "OpenCLWrapperHelper.h"

using namespace OpenCLWrapperHelper;

OpenCLWrapper::OpenCLWrapper(bool profile) : m_profile(profile) {
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
    if (m_profile) {
        qprops |= CL_QUEUE_PROFILING_ENABLE;
    }
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

void OpenCLWrapper::init_program_from_file(const std::string& kernel_file) {
    std::string kernel_code = read_file(kernel_file);
    init_program(kernel_code);
}

void OpenCLWrapper::build_program() {
    const char compiler_flags[] = "-cl-finite-math-only";
    //const char compiler_flags[] = "";
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
        char buffer[1024*8];
        clGetProgramBuildInfo(
                m_program,
                m_device,
                CL_PROGRAM_BUILD_LOG,
                1024*8,
                buffer,
                NULL);
        std::cout << buffer << std::endl;
    }
}

void OpenCLWrapper::init_kernels() {
    m_kernels.clear();
    cl_uint num_kernels;
    CALL_CL_GUARDED(clCreateKernelsInProgram, (m_program, 0, NULL, &num_kernels));
    std::vector<cl_kernel> kernels(num_kernels);
    CALL_CL_GUARDED(clCreateKernelsInProgram, (m_program, num_kernels, kernels.data(), NULL));
    for (std::vector<cl_kernel>::iterator itr = kernels.begin();
            itr != kernels.end(); itr++) {
        size_t name_len;
        CALL_CL_GUARDED(clGetKernelInfo, (*itr, CL_KERNEL_FUNCTION_NAME, 0, NULL, &name_len));
        char* name = new char[name_len];
        CALL_CL_GUARDED(clGetKernelInfo, (*itr, CL_KERNEL_FUNCTION_NAME, name_len, name, NULL));
        m_kernels[std::string(name)] = *itr;
        delete [] name;
    }
}

void OpenCLWrapper::set_kernel(const std::string& kernel_name) {
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
    cl_event complete_event;
    CALL_CL_GUARDED(clEnqueueReadBuffer,
            (m_queue, buf, CL_TRUE,
             offset, num_bytes, data,
             0, NULL, &complete_event));
    attach_profile_callback(complete_event);
}

void OpenCLWrapper::write_to_buffer(
        cl_mem buf, size_t offset, size_t num_bytes, void* data) {
    cl_event complete_event;
    CALL_CL_GUARDED(clEnqueueWriteBuffer,
            (m_queue, buf, CL_TRUE,
             offset, num_bytes, data,
             0, NULL, &complete_event));
    attach_profile_callback(complete_event);
}

size_t OpenCLWrapper::get_max_work_group_size() const {
    size_t max_work_group_size;
    CALL_CL_GUARDED(clGetKernelWorkGroupInfo, (
                m_kernel, m_device,
                CL_KERNEL_WORK_GROUP_SIZE,
                sizeof(size_t), &max_work_group_size,
                NULL));
    return max_work_group_size;
}

size_t OpenCLWrapper::get_preferred_work_group_size_multiple() const {
    size_t preferred_multiple;
    CALL_CL_GUARDED(clGetKernelWorkGroupInfo, (
                m_kernel, m_device,
                CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
                sizeof(size_t), &preferred_multiple,
                NULL));
    return preferred_multiple;
}

cl_ulong OpenCLWrapper::get_kernel_local_mem_size() const {
    cl_ulong local_size;
    CALL_CL_GUARDED(clGetKernelWorkGroupInfo, (
                m_kernel, m_device,
                CL_KERNEL_LOCAL_MEM_SIZE,
                sizeof(size_t), &local_size,
                NULL));
    return local_size;
}

cl_ulong OpenCLWrapper::get_kernel_private_mem_size() const {
    cl_ulong private_size;
    CALL_CL_GUARDED(clGetKernelWorkGroupInfo, (
                m_kernel, m_device,
                CL_KERNEL_PRIVATE_MEM_SIZE,
                sizeof(size_t), &private_size,
                NULL));
    return private_size;
}

void OpenCLWrapper::execute_kernel(size_t dim,
        size_t* global_work_size,
        size_t* local_work_size) {
    cl_event complete_event;
    CALL_CL_GUARDED(clEnqueueNDRangeKernel, (
                m_queue, m_kernel, dim,
                NULL, global_work_size, local_work_size,
                0, NULL, &complete_event));
    attach_profile_callback(complete_event);
    CALL_CL_GUARDED(clFinish, (m_queue));
}

void OpenCLWrapper::attach_profile_callback(cl_event& event) {
    if (m_profile) {
        CALL_CL_GUARDED(clSetEventCallback,
                (event, CL_COMPLETE, print_profile, NULL))
    }
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


