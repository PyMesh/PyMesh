#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Mesh.h>
#include <MeshFactory.h>
#include <IO/MeshWriter.h>

namespace py = pybind11;
using namespace PyMesh;
PYBIND11_MODULE(PyMesh, m) {
    m.doc() = "Basic PyMesh module.";

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

    py::class_<MeshFactory>(m, "MeshFactory")
        .def(py::init<>())
        .def("load_file", &MeshFactory::load_file,
                py::return_value_policy::reference_internal)
        .def("load_data", &MeshFactory::load_data,
                py::return_value_policy::reference_internal)
        .def("with_connectivity", &MeshFactory::with_connectivity,
                py::return_value_policy::reference_internal)
        .def("with_attribute", &MeshFactory::with_attribute,
                py::return_value_policy::reference_internal)
        .def("drop_zero_dim", &MeshFactory::drop_zero_dim,
                py::return_value_policy::reference_internal)
        .def("create", &MeshFactory::create);

    py::class_<MeshWriter>(m, "MeshWriter")
        .def(py::init<>())
        .def("create", &MeshWriter::create)
        .def("with_attribute", &MeshWriter::with_attribute)
        .def("in_ascii", &MeshWriter::in_ascii)
        .def("use_float", &MeshWriter::use_float)
        .def("write_mesh", &MeshWriter::write_mesh)
        .def("write", &MeshWriter::write)
        .def("set_output_filename", &MeshWriter::set_output_filename)
        .def("set_anonymous", &MeshWriter::set_anonymous)
        .def("is_anonymous", &MeshWriter::is_anonymous);
}
