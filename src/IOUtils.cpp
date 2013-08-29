#include "IOUtils.h"

std::string Zhou::get_extention(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    return filename.substr(pos);
}
