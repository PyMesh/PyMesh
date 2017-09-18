#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Mesh.h>
#include <MeshUtils/AttributeUtils.h>

namespace py = pybind11;
using namespace PyMesh;

void init_MeshUtils(py::module& m) {
    m.def("convert_to_vertex_attribute",
            &PyMesh::AttributeUtils::convert_to_vertex_attribute);
    m.def("convert_to_vertex_attribute_from_name",
            &PyMesh::AttributeUtils::convert_to_vertex_attribute_from_name);
    m.def("convert_to_face_attribute",
            &PyMesh::AttributeUtils::convert_to_face_attribute);
    m.def("convert_to_face_attribute_from_name",
            &PyMesh::AttributeUtils::convert_to_face_attribute_from_name);
    m.def("convert_to_voxel_attribute",
            &PyMesh::AttributeUtils::convert_to_voxel_attribute);
    m.def("convert_to_voxel_attribute_from_name",
            &PyMesh::AttributeUtils::convert_to_voxel_attribute_from_name);
}
