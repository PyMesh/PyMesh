/* This file is part of PyMesh. Copyright (c) 2020 by Julien Jerphanion */
#include "StatisticsEngine.h"

#ifdef WITH_CGAL
#include "CGAL/CGALStatisticsEngine.h"
#endif

#include <sstream>
#include <iostream>

using namespace PyMesh;

StatisticsEngine::Ptr StatisticsEngine::create(const std::string& engine_name) {
    if (engine_name == "auto") {
#ifdef WITH_CGAL
        return StatisticsEngine::create("cgal");
#endif
    }
#ifdef WITH_CGAL
    if (engine_name == "cgal") { return Ptr(new CGALStatisticsEngine()); }
#endif
    std::stringstream err_msg;
    err_msg << "Statistics engine \"" << engine_name
        << "\" is not supported." << std::endl;
    throw NotImplementedError(err_msg.str());
}

bool StatisticsEngine::supports(const std::string& engine_name) {
#if WITH_CGAL
    if (engine_name == "cgal") return true;
#endif
    return false;
}

std::vector<std::string> get_available_engines() {
    std::vector<std::string> engine_names;
#if WITH_CGAL
    engine_names.push_back("cgal");
#endif
    return engine_names;
}


void StatisticsEngine::serialize_xml(const std::string& filename) const {
    throw NotImplementedError("Serialization is not supported");
}

