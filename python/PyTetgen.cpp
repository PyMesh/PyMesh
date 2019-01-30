#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <TetGen/TetgenWrapper.h>

namespace py = pybind11;

void init_TetgenWrapper(py::module& m) {
#if WITH_TETGEN
    using namespace PyMesh;
    py::class_<TetgenWrapper, std::shared_ptr<TetgenWrapper> > tetgen(m, "tetgen");
    tetgen
        .def(py::init<>())
        .def_property("points",
                &TetgenWrapper::get_points,
                &TetgenWrapper::set_points,
                py::return_value_policy::reference_internal)
        .def_property("triangles",
                &TetgenWrapper::get_triangles,
                &TetgenWrapper::set_triangles,
                py::return_value_policy::reference_internal)
        .def_property("tetrahedra",
                &TetgenWrapper::get_tetrahedra,
                &TetgenWrapper::set_tetrahedra,
                py::return_value_policy::reference_internal)
        .def_property("point_markers",
                &TetgenWrapper::get_point_markers,
                &TetgenWrapper::set_point_markers,
                py::return_value_policy::reference_internal)
        .def_property("point_weights",
                &TetgenWrapper::get_point_weights,
                &TetgenWrapper::set_point_weights,
                py::return_value_policy::reference_internal)
        .def_property("triangle_markers",
                &TetgenWrapper::get_triangle_markers,
                &TetgenWrapper::set_triangle_markers,
                py::return_value_policy::reference_internal)
        .def_property("split_boundary",
                &TetgenWrapper::get_split_boundary,
                &TetgenWrapper::set_split_boundary)
        .def_property("max_radius_edge_ratio",
                &TetgenWrapper::get_max_radius_edge_ratio,
                &TetgenWrapper::set_max_radius_edge_ratio)
        .def_property("min_dihedral_angle",
                &TetgenWrapper::get_min_dihedral_angle,
                &TetgenWrapper::set_min_dihedral_angle)
        .def_property("coarsening",
                &TetgenWrapper::get_coarsening,
                &TetgenWrapper::set_coarsening)
        .def_property("max_tet_volume",
                &TetgenWrapper::get_max_tet_volume,
                &TetgenWrapper::set_max_tet_volume)
        .def_property("optimization_level",
                &TetgenWrapper::get_optimization_level,
                &TetgenWrapper::set_optimization_level)
        .def_property("max_num_steiner_points",
                &TetgenWrapper::get_max_num_steiner_points,
                &TetgenWrapper::set_max_num_steiner_points)
        .def_property("coplanar_tolerance",
                &TetgenWrapper::get_coplanar_tolerance,
                &TetgenWrapper::set_coplanar_tolerance)
        .def_property("exact_arithmetic",
                &TetgenWrapper::get_exact_arithmetic,
                &TetgenWrapper::set_exact_arithmetic)
        .def_property("merge_coplanar",
                &TetgenWrapper::get_merge_coplanar,
                &TetgenWrapper::set_merge_coplanar)
        .def_property("weighted_delaunay",
                &TetgenWrapper::get_weighted_delaunay,
                &TetgenWrapper::set_weighted_delaunay)
        .def_property("keep_convex_hull",
                &TetgenWrapper::get_keep_convex_hull,
                &TetgenWrapper::set_keep_convex_hull)
        .def_property("verbosity",
                &TetgenWrapper::get_verbosity,
                &TetgenWrapper::set_verbosity)
        .def("run", &TetgenWrapper::run)
        .def_property_readonly("vertices", &TetgenWrapper::get_vertices,
                py::return_value_policy::reference_internal)
        .def_property_readonly("faces", &TetgenWrapper::get_faces,
                py::return_value_policy::reference_internal)
        .def_property_readonly("voxels", &TetgenWrapper::get_voxels,
                py::return_value_policy::reference_internal)
        .def_property_readonly("regions", &TetgenWrapper::get_regions,
                py::return_value_policy::reference_internal)
        .def_property_readonly("flags",
                &TetgenWrapper::generate_command_line_options)
        ;
#endif
}
