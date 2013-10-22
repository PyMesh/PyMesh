#pragma once
#include <cstdlib>
#include <string>

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
};
