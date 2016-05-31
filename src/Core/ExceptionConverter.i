/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
%{
#include <Core/Exception.h>
%}
%include exception.i
%exception {
    try {
        $action
    } catch (const PyMesh::IOError& e) {
        SWIG_exception(SWIG_IOError, e.what());
    } catch (const PyMesh::RuntimeError& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    } catch (const std::exception& e) {
        SWIG_exception(SWIG_UnknownError, e.what());
    }
}
