/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "IOUtils.h"
#include <cstring>
#include <Core/Exception.h>

using namespace PyMesh;

std::string IOUtils::get_extention(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    return filename.substr(pos);
}

std::string IOUtils::get_name(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    return filename.substr(0, pos);
}

bool IOUtils::is_prefix(const char* prefix, const char* str) {
    return strncmp(prefix, str, strlen(prefix)) == 0;
}

std::string IOUtils::next_line(std::ifstream& fin) {
    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];
    do {
        if (fin.eof()) {
            throw IOError("Error parsing OFF file");
        }
        fin.getline(line, LINE_SIZE);
    } while(strlen(line) == 0 || line[0] == '#' ||
            line[0] == '\n' || line[0] == '\r');
    return std::string(line);
}
