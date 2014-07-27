#include "Functions.h"
#include <iostream>

using namespace Functions;
std::string Functions::output(const std::string& str) {
    return str;
}

std::vector<double> Functions::x2(const std::vector<double>& arr) {
    std::vector<double> result;
    for (auto entry : arr) {
        result.push_back(entry * 2);
    }
    return result;
}

std::vector<double> Functions::x3(const std::vector<double>& arr) {
    std::vector<double> result;
    for (auto entry : arr) {
        result.push_back(entry * 3);
    }
    return result;
}
