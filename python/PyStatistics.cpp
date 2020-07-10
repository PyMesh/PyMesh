#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Statistics/StatisticsEngine.h>

namespace py = pybind11;
using namespace PyMesh;

void init_Statistics(py::module& m) {
    py::class_<StatisticsEngine, std::shared_ptr<BooleanEngine> >(m, "StatisticsEngine")
        .def_static("create", &BooleanEngine::create)
        .def("set_mesh", &BooleanEngine::set_mesh)
        .def("get_vertices", &BooleanEngine::get_vertices)
        .def("get_faces", &BooleanEngine::get_faces)
        .def("compute_statistics", &BooleanEngine::compute_statistics)
        .def("serialize_xml", &BooleanEngine::serialize_xml);
}
