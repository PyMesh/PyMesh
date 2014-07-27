%{
#include "Functions.h"
%}
%include "std_string.i"
%include "std_vector.i"
namespace std {
    %template(vectord) vector<double>;
};
%pythonbegin %{
import numpy as np
%}
%pythonappend Functions::x3(const std::vector<double>&) %{
    val = np.array(val);
%}
%include "Functions.h"
%pythoncode %{
def output(name):
    name = "<" + name + ">";
    return _PySwigTests.output(name);
def x2(arr):
    result = _PySwigTests.x2(arr.tolist());
    return np.array(result);
%}
