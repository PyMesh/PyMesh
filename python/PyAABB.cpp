#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#if WITH_CGAL
#include <CGAL/AABBTree.h>
#include <CGAL/AABBTree2.h>
#endif
#include <BVH/BVHEngine.h>

namespace py = pybind11;
using namespace PyMesh;

void init_AABB(py::module &m) {
#if WITH_CGAL
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

    py::class_<AABBTree2, std::shared_ptr<AABBTree2> >(m, "AABBTree2")
        .def(py::init<const Matrix2Fr&, const Matrix2Ir&>())
        .def("look_up",
                [](AABBTree2& tree, const Matrix2Fr& points) {
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
                [](AABBTree2& tree, const Matrix2Fr& points) {
                VectorF squared_dists;
                VectorI closest_face_indices;
                Matrix2Fr closest_points;
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
#endif
}

void init_BVH(py::module &m) {
    py::class_<BVHEngine, std::shared_ptr<BVHEngine> >(m, "BVHEngine")
        .def(py::init<>())
        .def_static("create", &BVHEngine::create)
        .def_property_readonly_static("available_engines",
                [](py::object){return BVHEngine::get_available_engines();})
        .def("set_mesh", &BVHEngine::set_mesh)
        .def("build", &BVHEngine::build)
        .def("lookup",
                [](BVHEngine::Ptr tree, const MatrixFr& points) {
                VectorF squared_dists;
                VectorI closest_face_indices;
                MatrixFr closest_points;
                tree->lookup(
                        points,
                        squared_dists,
                        closest_face_indices,
                        closest_points);
                return std::make_tuple(
                        squared_dists,
                        closest_face_indices,
                        closest_points);
                })
        .def("lookup_signed",
                [](BVHEngine::Ptr tree, const MatrixFr& points, 
                  const MatrixFr& face_normals,
                  const MatrixFr& vertex_normals,
                  const MatrixFr& edge_normals,
                  const VectorI& edge_map) {
                VectorF signed_dists;
                VectorI closest_face_indices;
                MatrixFr closest_points, closest_face_normals;
                tree->lookup_signed(
                        points,
                        face_normals,
                        vertex_normals,
                        edge_normals,
                        edge_map,
                        signed_dists,
                        closest_face_indices,
                        closest_points,
                        closest_face_normals);
                return std::make_tuple(
                        signed_dists,
                        closest_face_indices,
                        closest_points,
                        closest_face_normals);
                });
}
