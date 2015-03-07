#include "SelfIntersectionResolver.h"

#include <sstream>
#include <Core/Exception.h>

#include <IGL/IGLSelfIntersectionResolver.h>

SelfIntersectionResolver::Ptr SelfIntersectionResolver::create(
        const std::string& engine_name) {
    if (engine_name == "igl") {
        return Ptr(new IGLSelfIntersectionResolver);
    } else {
        std::stringstream err_msg;
        err_msg << "Self-intersection engine \"" << engine_name
            << "\" is not supported";
        throw NotImplementedError(err_msg.str());
    }
}
