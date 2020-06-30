#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#include <Core/EigenTypedef.h>
#include <IGL/DiskCutter.h>
#include <IGL/HarmonicSolver.h>
#include <igl/unique_rows.h>
#include <igl/per_face_normals.h>
#include <igl/per_vertex_normals.h>
#include <igl/per_edge_normals.h>
#include <igl/orientable_patches.h>
#include <igl/orient_outward.h>

namespace py = pybind11;
using namespace PyMesh;

void init_IGL(py::module &m) {
#if WITH_IGL
    py::class_<HarmonicSolver, std::shared_ptr<HarmonicSolver> >(m, "HarmonicSolver")
        .def(py::init<const MatrixFr&, const MatrixIr&>())
        .def_static("create", &HarmonicSolver::create)
        .def_static("create_raw", &HarmonicSolver::create_raw)
        .def_property("nodes",
                &HarmonicSolver::get_nodes,
                &HarmonicSolver::set_nodes,
                py::return_value_policy::reference_internal)
        .def_property("elements",
                &HarmonicSolver::get_elements,
                &HarmonicSolver::set_elements,
                py::return_value_policy::reference_internal)
        .def_property("order",
                &HarmonicSolver::get_order,
                &HarmonicSolver::set_order)
        .def_property("boundary_indices",
                &HarmonicSolver::get_boundary_indices,
                &HarmonicSolver::set_boundary_indices,
                py::return_value_policy::reference_internal)
        .def_property("boundary_values",
                &HarmonicSolver::get_boundary_values,
                &HarmonicSolver::set_boundary_values,
                py::return_value_policy::reference_internal)
        .def("pre_process", &HarmonicSolver::pre_process)
        .def("solve", &HarmonicSolver::solve)
        .def_property_readonly("solution",
                &HarmonicSolver::get_solution,
                py::return_value_policy::reference_internal)
        ;

    py::class_<DiskCutter, std::shared_ptr<DiskCutter> >(m, "DiskCutter")
        .def(py::init<const MatrixFr&, const MatrixIr&>())
        .def_static("create", &DiskCutter::create)
        .def_static("create_raw", &DiskCutter::create_raw)
        .def("run", &DiskCutter::run)
        .def_property_readonly("vertices", &DiskCutter::get_vertices,
                py::return_value_policy::reference_internal)
        .def_property_readonly("faces", &DiskCutter::get_faces,
                py::return_value_policy::reference_internal);

    m.def("unique_rows",
            [](const MatrixFr& A) {
            MatrixFr C;
            VectorI IA, IC;
            igl::unique_rows(A, C, IA, IC);
            return std::make_tuple(C, IA, IC);
            });

    m.def("face_normals",
            [](const MatrixFr& V, const MatrixIr& F) {
            MatrixFr FN;
            igl::per_face_normals(V, F, FN);
            return FN;
            });

    m.def("vertex_normals",
            [](const MatrixFr& V, const MatrixIr& F, const MatrixFr& FN) {
            MatrixFr VN;
            igl::per_vertex_normals(
                V, F, igl::PER_VERTEX_NORMALS_WEIGHTING_TYPE_ANGLE, FN, VN);
            return VN;
            });

    m.def("edge_normals",
            [](const MatrixFr& V, const MatrixIr& F, const MatrixFr& FN) {
            MatrixFr EN;
            MatrixIr E;
            VectorI EMAP;
            igl::per_edge_normals(
                V, F, igl::PER_EDGE_NORMALS_WEIGHTING_TYPE_UNIFORM, FN, EN, E, EMAP);
            return std::make_tuple(EN, E, EMAP);
            });


    m.def("orient_outward",
            [](const MatrixFr& V, const MatrixIr& F) {
            VectorI C;
            igl::orientable_patches(F, C);
            MatrixIr FF;
            VectorI I;
            igl::orient_outward(V, F, C, FF, I);
            return FF;
            });
#endif
}
