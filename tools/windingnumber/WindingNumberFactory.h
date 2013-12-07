#pragma once
#include <string>

#include <Core/Exception.h>

#include "WindingNumber.h"
#include "WindingNumberWithHierarchy.h"
#include "WindingNumberWithOpenCL.h"
#include "WindingNumberWithOpenCL2.h"
#include "WindingNumberWithOpenCL3.h"

namespace WindingNumberFactory {
    WindingNumber* WindingNumberCalculator(const std::string& method) {
        if (method == "hierarchy") {
            return new WindingNumberWithHierarchy();
        } else if (method == "opencl") {
            return new WindingNumberWithOpenCL();
        } else if (method == "opencl2") {
            return new WindingNumberWithOpenCL2();
        } else if (method == "opencl3") {
            return new WindingNumberWithOpenCL3();
        } else {
            throw RuntimeError(
                    std::string("Unknow winding number computation method.")
                    + method);
        }
    }
}
