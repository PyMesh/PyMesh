%{
#include "FaceUtils.h"
%}
%include "std_vector.i"
namespace std {
    %template(vector_size_t) vector<size_t>;
};
%include "FaceUtils.h"
