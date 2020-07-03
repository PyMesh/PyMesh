#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Compression/CompressionEngine.h>

namespace py = pybind11;
using namespace PyMesh;

void init_Compression(py::module& m) {
    py::class_<CompressionEngine, std::shared_ptr<CompressionEngine> >(m, "CompressionEngine")
        .def_static("create", &CompressionEngine::create)
        .def_static("supports", &CompressionEngine::supports)
        .def_property_readonly_static("available_engines",
                [](py::object){
                return CompressionEngine::get_available_engines();})
        .def("compress",
                [](const std::shared_ptr<CompressionEngine> engine, PyMesh::Mesh::Ptr mesh) {
                const auto data = engine->compress(mesh);
                return py::bytes(data);
                })
        .def("decompress", &CompressionEngine::decompress);
}
