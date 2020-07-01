#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Core/EigenTypedef.h>
#include <IGL/DMAT.h>

namespace py = pybind11;
using namespace PyMesh;

void init_DMAT(py::module &m) {
#if WITH_IGL
    m.def("load_matrix", &DMAT::load_matrix<MatrixF>);
    m.def("save_matrix", &DMAT::save_matrix<MatrixF>);
#endif
}
