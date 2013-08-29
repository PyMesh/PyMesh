#pragma once
#include <iostream>
#include "EigenTypedef.h"

class CopyTest {
    public:
        void set_f_data(VectorF& data) { f_data = &data; }
        void set_i_data(VectorI& data) { i_data = &data; }

        void update_f_data(VectorF& data) { data[0] = -1.0; }
        void update_i_data(VectorI& data) { data[0] = -1; }

        void print_matrix(MatrixF& m);

        VectorF& get_f_data() { return *f_data; }
        VectorI& get_i_data() { return *i_data; }
    private:
        VectorF* f_data;
        VectorI* i_data;
};
