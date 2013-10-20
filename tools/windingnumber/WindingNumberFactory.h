#pragma once
#include <string>
#include <Exception.h>

#include "WindingNumber.h"
#include "WindingNumberWithHierarchy.h"
#include "WindingNumberWithOpenCL.h"

namespace WindingNumberFactory {
    WindingNumber* WindingNumberCalculator(const std::string& method) {
        if (method == "hierarchy") {
            return new WindingNumberWithHierarchy();
        } else if (method == "opencl") {
            return new WindingNumberWithOpenCL();
        } else {
            throw RuntimeError(
                    std::string("Unknow winding number computation method.")
                    + method);
        }
    }
}
