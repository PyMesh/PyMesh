#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Tetrahedralization/TetrahedralizationEngine.h>

namespace py = pybind11;
using namespace PyMesh;

void init_Tetrahedralization(py::module& m) {
    py::class_<TetrahedralizationEngine,
        std::shared_ptr<TetrahedralizationEngine> >(
                m, "TetrahedralizationEngine")
    .def_static("create", &TetrahedralizationEngine::create)
    .def("run", &TetrahedralizationEngine::run)
    .def("set_vertices", &TetrahedralizationEngine::set_vertices)
    .def("set_faces", &TetrahedralizationEngine::set_faces)
    .def("set_cell_radius_edge_ratio", &TetrahedralizationEngine::set_cell_radius_edge_ratio)
    .def("set_cell_size", &TetrahedralizationEngine::set_cell_size)
    .def("set_facet_distance", &TetrahedralizationEngine::set_facet_distance)
    .def("set_feature_angle", &TetrahedralizationEngine::set_feature_angle)
    .def("get_vertices", &TetrahedralizationEngine::get_vertices)
    .def("get_faces", &TetrahedralizationEngine::get_faces)
    .def("get_voxels", &TetrahedralizationEngine::get_voxels);
}
