#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <ConvexHull/ConvexHullEngine.h>

namespace py = pybind11;
using namespace PyMesh;

void init_ConvexHull(py::module& m) {
    py::class_<ConvexHullEngine>(m, "ConvexHullEngine")
        .def("run", &ConvexHullEngine::run)
        .def("get_vertices", &ConvexHullEngine::get_vertices)
        .def("get_faces", &ConvexHullEngine::get_faces)
        .def("get_index_map", &ConvexHullEngine::get_index_map);
}
