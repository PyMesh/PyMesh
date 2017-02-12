/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include <fstream>

namespace PyMesh {
namespace IOUtils {
    std::string get_extention(const std::string& filename);
    std::string get_name(const std::string& filename);
    bool is_prefix(const char* prefix, const char* str);
    std::string next_line(std::ifstream& fin);
}
}
