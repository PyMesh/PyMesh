#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Predicates/predicates.h>

namespace py = pybind11;

void init_predicates(py::module& m) {
    m.def("exactinit", &exactinit);
    m.def("orient2d", &orient2d);
    m.def("orient2dexact", &orient2dexact);
    m.def("orient3d", &orient3d);
    m.def("orient3dexact", &orient3dexact);
    m.def("incircle", &incircle);
    m.def("incircleexact", &incircleexact);
    m.def("insphere", &insphere);
    m.def("insphereexact", &insphereexact);
}
