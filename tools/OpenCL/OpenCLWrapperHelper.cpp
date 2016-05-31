#include "OpenCLWrapperHelper.h"
#include <iostream>

namespace PyMesh {
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

}
