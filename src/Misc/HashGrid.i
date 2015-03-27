/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
%{
#include <Misc/HashGrid.h>
%}
%include <std_shared_ptr.i>
%shared_ptr(HashGrid)
%include "HashGrid.h"
