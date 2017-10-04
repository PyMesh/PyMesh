#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <CGAL/SelfIntersection.h>

namespace py = pybind11;
using namespace PyMesh;

void init_CGAL(py::module &m) {
    py::class_<SelfIntersection, std::shared_ptr<SelfIntersection> >(m, "SelfIntersection")
        .def(py::init<const MatrixFr& , const MatrixIr&>())
        .def("detect_self_intersection",
                &SelfIntersection::detect_self_intersection)
        .def("clear", &SelfIntersection::clear)
        .def("get_self_intersecting_pairs",
                &SelfIntersection::get_self_intersecting_pairs)
        .def("handle_intersection_candidate",
                &SelfIntersection::handle_intersection_candidate);
}
