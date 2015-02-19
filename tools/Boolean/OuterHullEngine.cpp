#include "OuterHullEngine.h"
#include <Core/Exception.h>
#include "IGL/IGLOuterHullEngine.h"

#include <sstream>

OuterHullEngine::Ptr OuterHullEngine::create(const std::string& engine_name) {
    if (engine_name == "igl") {
        return std::make_shared<IGLOuterHullEngine>();
    } else {
        std::stringstream err_msg;
        err_msg << "Unknown outer hull engine: " << engine_name;
        throw NotImplementedError(err_msg.str());
    }
}
