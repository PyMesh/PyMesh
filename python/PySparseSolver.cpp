#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>

#include <SparseSolver/SparseSolver.h>

namespace py = pybind11;
using namespace PyMesh;

void init_SparseSolver(py::module& m) {
    py::class_<SparseSolver, std::shared_ptr<SparseSolver> >(m, "SparseSolver")
        .def_static("create", &SparseSolver::create)
        .def("analyze_pattern", &SparseSolver::analyze_pattern)
        .def("factorize", &SparseSolver::factorize)
        .def("solve", &SparseSolver::solve);
}
