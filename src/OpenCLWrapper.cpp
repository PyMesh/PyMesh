#include "OpenCLWrapper.h"
#include <fstream>
#include <iostream>
#include <Exception.h>

namespace OpenCLWrapperHelper {
    std::string get_event_type(cl_event event) {
        cl_command_type type;
        CALL_CL_GUARDED(clGetEventInfo,
                (event, CL_EVENT_COMMAND_TYPE, sizeof(type), &type, NULL));
        std::string result;
        switch (type) {
            case CL_COMMAND_NDRANGE_KERNEL:
                result = "NDRANGE_KERNEL";
                break;
            case CL_COMMAND_TASK:
                result = "TASK";
                break;
            case CL_COMMAND_NATIVE_KERNEL:
                result = "NATIVE_KERNEL";
                break;
            case CL_COMMAND_READ_BUFFER:
                result = "READ_BUFFER";
                break;
            case CL_COMMAND_WRITE_BUFFER:
                result = "WRITE_BUFFER";
                break;
            case CL_COMMAND_COPY_BUFFER:
                result =  "COPY_BUFFER";
                break;
            case CL_COMMAND_READ_IMAGE:
                result =  "READ_IMAGE";
                break;
            case CL_COMMAND_WRITE_IMAGE:
                result =  "WRITE_IMAGE";
                break;
            case CL_COMMAND_COPY_IMAGE:
                result =  "COPY_IMAGE";
                break;
            case CL_COMMAND_COPY_BUFFER_TO_IMAGE:
                result =  "COPY_BUFFER_TO_IMAGE";
                break;
            case CL_COMMAND_COPY_IMAGE_TO_BUFFER:
                result =  "COPY_IMAGE_TO_BUFFER";
                break;
            case CL_COMMAND_MAP_BUFFER:
                result =  "MAP_BUFFER";
                break;
            case CL_COMMAND_MAP_IMAGE:
                result =  "MAP_IMAGE";
                break;
            case CL_COMMAND_UNMAP_MEM_OBJECT:
                result =  "UNMAP_MEM_OBJECT";
                break;
            case CL_COMMAND_MARKER:
                result =  "MARKER";
                break;
            case CL_COMMAND_ACQUIRE_GL_OBJECTS:
                result =  "ACQUIRE_GL_OBJECTS";
                break;
            case CL_COMMAND_RELEASE_GL_OBJECTS:
                result = "RELEASE_GL_OBJECTS";
                break;
        }
        return result;
    }

    void print_profile(cl_event event, cl_int status, void* data) {
        cl_long t_enqueue, t_submit, t_start, t_end;
        CALL_CL_GUARDED(clGetEventProfilingInfo,
                (event, CL_PROFILING_COMMAND_QUEUED,
                sizeof(t_enqueue), &t_enqueue, NULL));
        CALL_CL_GUARDED(clGetEventProfilingInfo,
                (event, CL_PROFILING_COMMAND_SUBMIT,
                sizeof(t_submit), &t_submit, NULL));
        CALL_CL_GUARDED(clGetEventProfilingInfo,
                (event, CL_PROFILING_COMMAND_START,
                sizeof(t_start), &t_start, NULL));
        CALL_CL_GUARDED(clGetEventProfilingInfo,
                (event, CL_PROFILING_COMMAND_END,
                sizeof(t_end), &t_end, NULL));
        std::cout << get_event_type(event) << "  status: " << status << std::endl;
        std::cout << "time on queue : "
            << (t_submit - t_enqueue) / 1.0e9 << "s" << std::endl;
        std::cout << "time submision: "
            << (t_start - t_submit) / 1.0e9 << "s" << std::endl;
        std::cout << "time execution: "
            << (t_end - t_start) / 1.0e9 << "s" << std::endl;
    }
}

using namespace OpenCLWrapperHelper;

OpenCLWrapper::OpenCLWrapper(bool profile) : m_profile(profile), m_work_group_size(0) {
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

size_t OpenCLWrapper::get_kernel_work_group_size() const {
    if (m_work_group_size > 0) return m_work_group_size;

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
    cl_event complete_event;
    size_t work_group_size = get_kernel_work_group_size();
    CALL_CL_GUARDED(clEnqueueNDRangeKernel, (
                m_queue, m_kernel,
                1, NULL, &num_work_items, &work_group_size,
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


