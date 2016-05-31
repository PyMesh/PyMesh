/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <cstdlib>
#include <string>
#include <Core/Exception.h>

namespace PyMesh {

/**
 * A simple wrapper class to access environment variables.
 * Usage:
 * std::string home = Environment::get("HOME");
 */
class Environment {
    public:
        static std::string get(const std::string& key) {
            char * val = getenv(key.c_str());
            return std::string(val == NULL ? "" : val);
        }

        static std::string get_required(const std::string& key) {
            char * val = getenv(key.c_str());
            if (val == NULL) {
                std::string msg = "Environment variable (";
                msg += key;
                msg += ") does not exist!";
                throw RuntimeError(msg);
            }
            return std::string(val);
        }
};

}
