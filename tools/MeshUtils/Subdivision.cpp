#include "Subdivision.h"
#include "SimpleSubdivision.h"

#include <sstream>

#include <Core/Exception.h>

Subdivision::Ptr Subdivision::create(const std::string& type) {
    if (type == "simple") {
        return Ptr(new SimpleSubdivision());
    } else {
        std::stringstream err_msg;
        err_msg << "Invalid subdivision type: " << type;
        throw NotImplementedError(err_msg.str());
    }
}
