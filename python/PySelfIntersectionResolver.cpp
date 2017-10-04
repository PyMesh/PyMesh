#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <SelfIntersection/SelfIntersectionResolver.h>

namespace py = pybind11;
using namespace PyMesh;

void init_SelfIntersectionResolver(py::module& m) {
    py::class_<SelfIntersectionResolver,
        std::shared_ptr<SelfIntersectionResolver> >(
                m, "SelfIntersectionResolver")
    .def_static("create", &SelfIntersectionResolver::create)
    .def("set_mesh", &SelfIntersectionResolver::set_mesh)
    .def("run", &SelfIntersectionResolver::run)
    .def("get_vertices", &SelfIntersectionResolver::get_vertices)
    .def("get_faces", &SelfIntersectionResolver::get_faces)
    .def("get_face_sources", &SelfIntersectionResolver::get_face_sources);
}
