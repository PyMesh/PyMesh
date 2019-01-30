#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Triangulation/Triangulation.h>

namespace py = pybind11;
using namespace PyMesh;

void init_Triangulation(py::module& m) {
    py::class_<Triangulation, std::shared_ptr<Triangulation> >(m, "Triangulation")
        .def_static("create", &Triangulation::create)
        .def("set_points", &Triangulation::set_points)
        .def("set_segments", &Triangulation::set_segments)
        .def("set_vertices", &Triangulation::set_vertices)
        .def("set_faces", &Triangulation::set_faces)
        .def("run", &Triangulation::run)
        .def("refine", &Triangulation::refine)
        .def("get_vertices", &Triangulation::get_vertices)
        .def("get_faces", &Triangulation::get_faces);
}
