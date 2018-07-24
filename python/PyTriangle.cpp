#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Triangle/TriangleWrapper.h>

namespace py = pybind11;
using namespace PyMesh;

void init_TriangleWrapper(py::module& m) {
    py::class_<TriangleWrapper, std::shared_ptr<TriangleWrapper> > triangle(m, "triangle");
    triangle
        .def(py::init<>())
        .def_property("points", &TriangleWrapper::get_points, &TriangleWrapper::set_points,
                py::return_value_policy::reference_internal)
        .def_property("segments", &TriangleWrapper::get_segments, &TriangleWrapper::set_segments,
                py::return_value_policy::reference_internal)
        .def_property("triangles", &TriangleWrapper::get_triangles, &TriangleWrapper::set_triangles,
                py::return_value_policy::reference_internal)
        .def_property("holes", &TriangleWrapper::get_holes, &TriangleWrapper::set_holes,
                py::return_value_policy::reference_internal)
        .def_property("min_angle", &TriangleWrapper::get_min_angle, &TriangleWrapper::set_min_angle)
        .def_property("max_area", &TriangleWrapper::get_max_area, &TriangleWrapper::set_max_area)
        .def_property("max_areas", &TriangleWrapper::get_max_area_list, &TriangleWrapper::set_max_area_list)
        .def_property("keep_convex_hull", &TriangleWrapper::get_keep_convex_hull, &TriangleWrapper::keep_convex_hull)
        .def_property("conforming_delaunay", &TriangleWrapper::get_conforming_delaunay, &TriangleWrapper::set_conforming_delaunay)
        .def_property("exact_arithmetic", &TriangleWrapper::get_exact_arithmetic, &TriangleWrapper::set_exact_arithmetic)
        .def_property("split_boundary", &TriangleWrapper::get_split_boundary, &TriangleWrapper::set_split_boundary)
        .def_property("max_num_steiner_points", &TriangleWrapper::get_max_num_steiner_points, &TriangleWrapper::set_max_num_steiner_points)
        .def_property("verbosity", &TriangleWrapper::get_verbosity, &TriangleWrapper::set_verbosity)
        .def_property("algorithm", &TriangleWrapper::get_algorithm, &TriangleWrapper::set_algorithm)
        .def_property("auto_hole_detection", &TriangleWrapper::get_auto_hole_detection, &TriangleWrapper::set_auto_hole_detection)
        .def("run", &TriangleWrapper::run)
        .def_property_readonly("vertices", &TriangleWrapper::get_vertices,
                py::return_value_policy::reference_internal)
        .def_property_readonly("faces", &TriangleWrapper::get_faces,
                py::return_value_policy::reference_internal)
        .def_property_readonly("voronoi_vertices", &TriangleWrapper::get_voronoi_vertices,
                py::return_value_policy::reference_internal)
        .def_property_readonly("voronoi_edges", &TriangleWrapper::get_voronoi_edges,
                py::return_value_policy::reference_internal)
        .def_property_readonly("regions", &TriangleWrapper::get_regions,
                py::return_value_policy::reference_internal);
    py::enum_<TriangleWrapper::Algorithm>(triangle, "Algorithm")
        .value("DIVIDE_AND_CONQUER", TriangleWrapper::Algorithm::DIVIDE_AND_CONQUER)
        .value("SWEEPLINE", TriangleWrapper::Algorithm::SWEEPLINE)
        .value("INCREMENTAL", TriangleWrapper::Algorithm::INCREMENTAL);
}
