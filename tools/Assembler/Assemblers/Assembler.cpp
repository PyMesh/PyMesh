#include "Assembler.h"

#include <sstream>

#include <Core/Exception.h>

#include "StiffnessAssembler.h"

Assembler::Ptr Assembler::create(const std::string& matrix_name) {
    if (matrix_name == "stiffness") {
        return Ptr(new StiffnessAssembler());
    } else {
        std::stringstream err_msg;
        err_msg << "Assembling " << matrix_name << " is not supported yet.";
        throw NotImplementedError(err_msg.str());
    }
}
