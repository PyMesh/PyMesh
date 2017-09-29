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
        .def("look_up",
                [](AABBTree& tree, const MatrixFr& points) {
                VectorF squared_dists;
                VectorI closest_face_indices;
                tree.look_up(
                        points,
                        squared_dists,
                        closest_face_indices);
                return std::make_tuple(
                        squared_dists,
                        closest_face_indices);
                })
        .def("look_up_with_closest_points",
                [](AABBTree& tree, const MatrixFr& points) {
                VectorF squared_dists;
                VectorI closest_face_indices;
                MatrixFr closest_points;
                tree.look_up_with_closest_points(
                        points,
                        squared_dists,
                        closest_face_indices,
                        closest_points);
                return std::make_tuple(
                        squared_dists,
                        closest_face_indices,
                        closest_points);
                });
}
