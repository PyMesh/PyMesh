%{
#include "Mesh.h"
%}
%include "std_string.i"
%include "std_vector.i"
namespace std {
    %template(vectori) vector<int>;
    %template(vectord) vector<double>;
    %template(vectors) vector<string>;
};
%include "Mesh.h"
