#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <WindingNumber/WindingNumberEngine.h>

namespace py = pybind11;
using namespace PyMesh;

void init_WindingNumber(py::module &m) {
    py::class_<WindingNumberEngine, std::shared_ptr<WindingNumberEngine> >(
            m, "WindingNumberEngine")
        .def_static("create", &WindingNumberEngine::create)
        .def("run", &WindingNumberEngine::run)
        .def("set_mesh", &WindingNumberEngine::set_mesh);
}
