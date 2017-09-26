#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Tetrahedronization/TetrahedronizationEngine.h>

namespace py = pybind11;
using namespace PyMesh;

void init_Tetrahedralization(py::module& m) {
    py::class_<TetrahedronizationEngine,
        std::shared_ptr<TetrahedronizationEngine> >(
                m, "TetrahedralizationEngine")
    .def("create", &TetrahedronizationEngine::create)
    .def("run", &TetrahedronizationEngine::run)
    .def("set_vertices", &TetrahedronizationEngine::set_vertices)
    .def("set_faces", &TetrahedronizationEngine::set_faces)
    .def("set_edge_size", &TetrahedronizationEngine::set_edge_size)
    .def("set_face_size", &TetrahedronizationEngine::set_face_size)
    .def("set_cell_radius_edge_ratio", &TetrahedronizationEngine::set_cell_radius_edge_ratio)
    .def("set_cell_size", &TetrahedronizationEngine::set_cell_size)
    .def("get_vertices", &TetrahedronizationEngine::get_vertices)
    .def("get_faces", &TetrahedronizationEngine::get_faces)
    .def("get_voxels", &TetrahedronizationEngine::get_voxels);
}
