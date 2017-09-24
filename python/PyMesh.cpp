#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void init_Mesh(py::module&);
void init_MeshFactory(py::module&);
void init_MeshWriter(py::module&);
void init_MeshUtils(py::module&);
void init_predicates(py::module&);
void init_ConvexHull(py::module&);

PYBIND11_MODULE(PyMesh, m) {
    m.doc() = "Basic PyMesh module.";

    init_Mesh(m);
    init_MeshFactory(m);
    init_MeshWriter(m);

    init_MeshUtils(m);
    init_predicates(m);
    init_ConvexHull(m);
}
