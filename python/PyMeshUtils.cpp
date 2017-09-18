#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Mesh.h>
#include <MeshUtils/AttributeUtils.h>
#include <MeshUtils/ObtuseTriangleRemoval.h>
#include <MeshUtils/ShortEdgeRemoval.h>
#include <MeshUtils/MeshUtils.h>

namespace py = pybind11;
using namespace PyMesh;

void init_MeshUtils(py::module& m) {
    m.def("convert_to_vertex_attribute",
            &AttributeUtils::convert_to_vertex_attribute);
    m.def("convert_to_vertex_attribute_from_name",
            &AttributeUtils::convert_to_vertex_attribute_from_name);
    m.def("convert_to_face_attribute",
            &AttributeUtils::convert_to_face_attribute);
    m.def("convert_to_face_attribute_from_name",
            &AttributeUtils::convert_to_face_attribute_from_name);
    m.def("convert_to_voxel_attribute",
            &AttributeUtils::convert_to_voxel_attribute);
    m.def("convert_to_voxel_attribute_from_name",
            &AttributeUtils::convert_to_voxel_attribute_from_name);
    m.def("extract_exterior_faces",
            &MeshUtils::extract_exterior_faces);

    py::class_<ObtuseTriangleRemoval>(m, "ObtuseTriangleRemoval")
        .def(py::init<MatrixFr&, MatrixIr&>())
        .def("run", &ObtuseTriangleRemoval::run)
        .def("get_vertices", &ObtuseTriangleRemoval::get_vertices)
        .def("get_faces", &ObtuseTriangleRemoval::get_faces);

    py::class_<ShortEdgeRemoval>(m, "ShortEdgeRemoval")
        .def(py::init<const MatrixFr&, const MatrixIr&>())
        .def("set_importance", &ShortEdgeRemoval::set_importance)
        .def("run", &ShortEdgeRemoval::run)
        .def("get_vertices", &ShortEdgeRemoval::get_vertices)
        .def("get_faces", &ShortEdgeRemoval::get_faces)
        .def("get_face_indices", &ShortEdgeRemoval::get_face_indices);

    //py::class_<MeshSeparator>(m, "MeshSeparator")
    //    .def(py::init<const MatrixI& elements>);
}
