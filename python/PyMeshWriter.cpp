#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Mesh.h>
#include <IO/MeshWriter.h>

namespace py = pybind11;
using namespace PyMesh;

void init_MeshWriter(py::module &m) {
    py::class_<MeshWriter, std::shared_ptr<MeshWriter> >(m, "MeshWriter")
        .def(py::init<>())
        .def_static("create", &MeshWriter::create)
        .def("with_attribute", &MeshWriter::with_attribute)
        .def("in_ascii", &MeshWriter::in_ascii)
        .def("use_float", &MeshWriter::use_float)
        .def("write_mesh", &MeshWriter::write_mesh)
        .def("write", &MeshWriter::write)
        .def("set_output_filename", &MeshWriter::set_output_filename)
        .def("set_anonymous", &MeshWriter::set_anonymous)
        .def("is_anonymous", &MeshWriter::is_anonymous);
}

