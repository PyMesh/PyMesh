#pragma once
#include <string>
extern "C" {
#include "cl-helper.h"
}

namespace OpenCLWrapperHelper {
    std::string get_event_type(cl_event event);
    void print_profile(cl_event event, cl_int status, void* data);
}
