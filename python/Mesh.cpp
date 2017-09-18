#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Mesh.h>

namespace py = pybind11;
using namespace PyMesh;

void init_Mesh(py::module &m) {
    py::class_<Mesh, std::shared_ptr<Mesh> >(m, "Mesh")
        .def("get_dim", &Mesh::get_dim)
        .def("get_num_vertices", &Mesh::get_num_vertices)
        .def("get_num_faces", &Mesh::get_num_faces)
        .def("get_num_voxels", &Mesh::get_num_voxels)
        .def("get_vertex_per_face", &Mesh::get_vertex_per_face)
        .def("get_vertex_per_voxel", &Mesh::get_vertex_per_voxel)
        .def("get_vertices", py::overload_cast<>(&Mesh::get_vertices, py::const_),
                py::return_value_policy::reference_internal)
        .def("get_faces", py::overload_cast<>(&Mesh::get_faces, py::const_),
                py::return_value_policy::reference_internal)
        .def("get_voxels", py::overload_cast<>(&Mesh::get_voxels, py::const_),
                py::return_value_policy::reference_internal)
        .def("enable_connectivity", &Mesh::enable_connectivity)
        .def("enable_vertex_connectivity", &Mesh::enable_vertex_connectivity)
        .def("enable_face_connectivity", &Mesh::enable_face_connectivity)
        .def("enable_voxel_connectivity", &Mesh::enable_voxel_connectivity)
        .def("get_vertex_adjacent_vertices", &Mesh::get_vertex_adjacent_vertices,
                py::return_value_policy::move)
        .def("get_vertex_adjacent_faces", &Mesh::get_vertex_adjacent_faces,
                py::return_value_policy::move)
        .def("get_vertex_adjacent_voxels", &Mesh::get_vertex_adjacent_voxels,
                py::return_value_policy::move)
        .def("get_face_adjacent_faces", &Mesh::get_face_adjacent_faces,
                py::return_value_policy::move)
        .def("get_face_adjacent_voxels", &Mesh::get_face_adjacent_voxels,
                py::return_value_policy::move)
        .def("get_voxel_adjacent_faces", &Mesh::get_voxel_adjacent_faces,
                py::return_value_policy::move)
        .def("get_voxel_adjacent_voxels", &Mesh::get_voxel_adjacent_voxels,
                py::return_value_policy::move)
        .def("has_attribute", &Mesh::has_attribute)
        .def("add_attribute", &Mesh::add_attribute)
        .def("remove_attribute", &Mesh::remove_attribute)
        .def("get_attribute", py::overload_cast<const std::string&>(&Mesh::get_attribute, py::const_),
                py::return_value_policy::reference_internal)
        .def("set_attribute", &Mesh::set_attribute)
        .def("get_attribute_names", &Mesh::get_attribute_names);
}
