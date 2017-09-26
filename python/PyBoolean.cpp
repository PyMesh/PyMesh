#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Boolean/BooleanEngine.h>

namespace py = pybind11;
using namespace PyMesh;

void init_Boolean(py::module& m) {
    py::class_<BooleanEngine, std::shared_ptr<BooleanEngine> >(m, "BooleanEngine")
        .def("create", &BooleanEngine::create)
        .def("set_mesh_1", &BooleanEngine::set_mesh_1)
        .def("set_mesh_2", &BooleanEngine::set_mesh_2)
        .def("get_vertices", &BooleanEngine::get_vertices)
        .def("get_faces", &BooleanEngine::get_faces)
        .def("clean_up", &BooleanEngine::clean_up)
        .def("compute_union", &BooleanEngine::compute_union)
        .def("compute_intersection", &BooleanEngine::compute_intersection)
        .def("compute_difference", &BooleanEngine::compute_difference)
        .def("compute_symmetric_difference",
                &BooleanEngine::compute_symmetric_difference)
        .def("get_face_sources", &BooleanEngine::get_face_sources)
        .def("serialize_xml", &BooleanEngine::serialize_xml);
}
