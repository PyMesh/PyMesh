/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
%{
#include "MshLoader.h"
%}
%include "std_vector.i"
%include "std_string.i"
namespace std {
    %template(field_names) vector<string>;
}
%include "MshLoader.h"
