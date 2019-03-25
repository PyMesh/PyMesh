#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Mesh.h>
#include <MeshFactory.h>

namespace py = pybind11;
using namespace PyMesh;

void init_MeshFactory(py::module &m) {
    py::class_<MeshFactory>(m, "MeshFactory")
        .def(py::init<>())
        .def("load_file", &MeshFactory::load_file,
                py::return_value_policy::reference_internal)
        .def("load_file_with_hint", &MeshFactory::load_file_with_hint,
             py::return_value_policy::reference_internal)
        .def("load_data", &MeshFactory::load_data,
                py::return_value_policy::reference_internal)
        .def("load_matrices", &MeshFactory::load_matrices,
                py::return_value_policy::reference_internal)
        .def("with_connectivity", &MeshFactory::with_connectivity,
                py::return_value_policy::reference_internal)
        .def("with_attribute", &MeshFactory::with_attribute,
                py::return_value_policy::reference_internal)
        .def("drop_zero_dim", &MeshFactory::drop_zero_dim,
                py::return_value_policy::reference_internal)
        .def("create", &MeshFactory::create);
}
