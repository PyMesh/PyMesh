%{
#include "Exception.h"
%}
%include exception.i
%exception {
    try {
        $action
    } catch (const IOError& e) {
        SWIG_exception(SWIG_IOError, e.what());
    } catch (const RuntimeError& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    } catch (const std::exception& e) {
        SWIG_exception(SWIG_UnknownError, e.what());
    }
}
