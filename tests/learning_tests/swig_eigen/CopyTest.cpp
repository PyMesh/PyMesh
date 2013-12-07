#include "CopyTest.h"
#include <iostream>

void CopyTest::print_matrix(MatrixF& m) {
    std::cout << m << std::endl;
}

void CopyTest::print_f_data() const {
    std::cout << f_data << std::endl;
}

void CopyTest::print_i_data() const {
    std::cout << i_data << std::endl;
}
