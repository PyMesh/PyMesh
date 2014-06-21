#include "IOUtils.h"
#include <cstring>

std::string IOUtils::get_extention(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    return filename.substr(pos);
}

bool IOUtils::is_prefix(const char* prefix, const char* str) {
    return strncmp(prefix, str, strlen(prefix)) == 0;
}
