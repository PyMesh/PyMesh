#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <CGAL/AABBTree.h>

namespace py = pybind11;
using namespace PyMesh;

void init_AABB(py::module &m) {
    py::class_<AABBTree, std::shared_ptr<AABBTree> >(m, "AABBTree")
        .def(py::init<const MatrixFr&, const MatrixIr&>())
        .def("do_intersect_segments", &AABBTree::do_intersect_segments)
        .def("look_up", &AABBTree::look_up)
        .def("look_up_with_closest_points", &AABBTree::look_up_with_closest_points);
}
