#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Mesh.h>
#include <MeshUtils/AttributeUtils.h>
#include <MeshUtils/EdgeUtils.h>
#include <MeshUtils/ObtuseTriangleRemoval.h>
#include <MeshUtils/ShortEdgeRemoval.h>
#include <MeshUtils/ManifoldCheck.h>
#include <MeshUtils/MeshCutter.h>
#include <MeshUtils/MeshUtils.h>
#include <MeshUtils/MeshSeparator.h>
#include <MeshUtils/MeshChecker.h>
#include <MeshUtils/Boundary.h>
#include <MeshUtils/PointLocator.h>
#include <MeshUtils/Subdivision.h>
#include <MeshUtils/DuplicatedVertexRemoval.h>
#include <MeshUtils/IsolatedVertexRemoval.h>
#include <MeshUtils/LongEdgeRemoval.h>
#include <MeshUtils/FinFaceRemoval.h>
#include <MeshUtils/DegeneratedTriangleRemoval.h>
#include <MeshUtils/FaceUtils.h>
#include <MeshUtils/VoxelUtils.h>

namespace py = pybind11;
using namespace PyMesh;
using namespace pybind11::literals;

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
    m.def("is_colinear_2D", &FaceUtils::is_colinear_2D);
    m.def("is_colinear_3D", &FaceUtils::is_colinear_3D);
    m.def("get_degenerated_faces", &FaceUtils::get_degenerated_faces);
    m.def("get_triangle_orientations", &FaceUtils::get_triangle_orientations);
    m.def("get_tet_orientations", &VoxelUtils::get_tet_orientations);
    m.def("is_delaunay", &VoxelUtils::is_delaunay);
    m.def("is_vertex_manifold", &ManifoldCheck::is_vertex_manifold);
    m.def("is_edge_manifold", &ManifoldCheck::is_edge_manifold);
    m.def("cut_to_manifold", &ManifoldCheck::cut_to_manifold);
    m.def("chain_edges", &EdgeUtils::chain_edges);

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

    py::class_<MeshSeparator> separator(m, "MeshSeparator");
    separator.def(py::init<const MatrixI&>())
        .def("set_connectivity_type", &MeshSeparator::set_connectivity_type)
        .def("separate", &MeshSeparator::separate)
        .def("get_component", &MeshSeparator::get_component)
        .def("get_sources", &MeshSeparator::get_sources)
        .def("clear", &MeshSeparator::clear);

    py::enum_<MeshSeparator::ConnectivityType>(separator, "ConnectivityType")
        .value("VERTEX", MeshSeparator::ConnectivityType::VERTEX)
        .value("FACE", MeshSeparator::ConnectivityType::FACE)
        .value("VOXEL", MeshSeparator::ConnectivityType::VOXEL)
        .export_values();

    py::class_<MeshChecker>(m, "MeshChecker")
        .def(py::init<const MatrixFr&, const MatrixIr&, const MatrixIr&>())
        .def("is_vertex_manifold", &MeshChecker::is_vertex_manifold)
        .def("is_edge_manifold", &MeshChecker::is_edge_manifold)
        .def("is_closed", &MeshChecker::is_closed)
        .def("has_edge_with_odd_adj_faces",
                &MeshChecker::has_edge_with_odd_adj_faces)
        .def("is_oriented", &MeshChecker::is_oriented)
        .def("has_complex_boundary", &MeshChecker::has_complex_boundary)
        .def("get_num_boundary_edges", &MeshChecker::get_num_boundary_edges)
        .def("get_num_boundary_loops", &MeshChecker::get_num_boundary_loops)
        .def("get_boundary_edges", &MeshChecker::get_boundary_edges)
        .def("get_boundary_loops", &MeshChecker::get_boundary_loops)
        .def("get_genus", &MeshChecker::get_genus)
        .def("get_euler_characteristic",
                &MeshChecker::get_euler_characteristic)
        .def("get_num_connected_components",
                &MeshChecker::get_num_connected_components)
        .def("get_num_connected_surface_components",
                &MeshChecker::get_num_connected_surface_components)
        .def("get_num_connected_volume_components",
                &MeshChecker::get_num_connected_volume_components)
        .def("get_num_isolated_vertices",
                &MeshChecker::get_num_isolated_vertices)
        .def("get_num_duplicated_faces",
                &MeshChecker::get_num_duplicated_faces)
        .def("compute_signed_volume_from_surface",
                &MeshChecker::compute_signed_volume_from_surface);

    py::class_<PointLocator>(m, "PointLocator")
        .def(py::init<Mesh::Ptr>())
        .def("locate", &PointLocator::locate)
        .def("get_enclosing_voxels", &PointLocator::get_enclosing_voxels)
        .def("get_barycentric_coords", &PointLocator::get_barycentric_coords)
        .def("clear", &PointLocator::clear);

    py::class_<Subdivision, std::shared_ptr<Subdivision> >(m, "Subdivision")
        .def_static("create", &Subdivision::create)
        .def("subdivide", &Subdivision::subdivide)
        .def("get_subdivision_matrices()",
                &Subdivision::get_subdivision_matrices)
        .def("get_vertices", &Subdivision::get_vertices)
        .def("get_faces", &Subdivision::get_faces)
        .def("get_face_indices", &Subdivision::get_face_indices)
        .def("get_num_vertices", &Subdivision::get_num_vertices)
        .def("get_num_faces", &Subdivision::get_num_faces);

    py::class_<DuplicatedVertexRemoval>(m, "DuplicatedVertexRemoval")
        .def(py::init<const MatrixFr&, const MatrixIr&>())
        .def("run", &DuplicatedVertexRemoval::run)
        .def("get_vertices", &DuplicatedVertexRemoval::get_vertices)
        .def("get_faces", &DuplicatedVertexRemoval::get_faces)
        .def("set_importance_level",
                &DuplicatedVertexRemoval::set_importance_level)
        .def("get_index_map", &DuplicatedVertexRemoval::get_index_map);

    py::class_<IsolatedVertexRemoval>(m, "IsolatedVertexRemoval")
        .def(py::init<const MatrixFr&, const MatrixIr&>())
        .def("run", &IsolatedVertexRemoval::run)
        .def("get_vertices", &IsolatedVertexRemoval::get_vertices)
        .def("get_faces", &IsolatedVertexRemoval::get_faces)
        .def("get_ori_vertex_indices",
                &IsolatedVertexRemoval::get_ori_vertex_indices);

    py::class_<LongEdgeRemoval>(m, "LongEdgeRemoval")
        .def(py::init<const MatrixFr&, const MatrixIr&>())
        .def("run", &LongEdgeRemoval::run, "max_length"_a, "recursive"_a=true)
        .def("get_vertices", &LongEdgeRemoval::get_vertices)
        .def("get_faces", &LongEdgeRemoval::get_faces)
        .def("get_ori_faces", &LongEdgeRemoval::get_ori_faces);

    py::class_<FinFaceRemoval>(m, "FinFaceRemoval")
        .def(py::init<const MatrixFr&, const MatrixIr&>())
        .def("set_fins_only", &FinFaceRemoval::set_fins_only)
        .def("run", &FinFaceRemoval::run)
        .def("get_vertices", &FinFaceRemoval::get_vertices)
        .def("get_faces", &FinFaceRemoval::get_faces)
        .def("get_face_indices", &FinFaceRemoval::get_face_indices);

    py::class_<DegeneratedTriangleRemoval>(m, "DegeneratedTriangleRemoval")
        .def(py::init<const MatrixFr&, const MatrixIr&>())
        .def("run", &DegeneratedTriangleRemoval::run)
        .def("get_vertices", &DegeneratedTriangleRemoval::get_vertices)
        .def("get_faces", &DegeneratedTriangleRemoval::get_faces)
        .def("get_ori_face_indices",
                &DegeneratedTriangleRemoval::get_ori_face_indices);

    py::class_<MeshCutter>(m, "MeshCutter")
        .def(py::init<Mesh::Ptr>())
        .def("cut_with_face_labels", &MeshCutter::cut_with_face_labels)
        .def("cut_at_uv_discontinuity", &MeshCutter::cut_at_uv_discontinuity);
}

