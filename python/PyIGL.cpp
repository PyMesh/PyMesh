#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#include <Core/EigenTypedef.h>
#include <IGL/HarmonicSolver.h>

namespace py = pybind11;
using namespace PyMesh;

void init_IGL(py::module &m) {
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
}
