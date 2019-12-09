#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Mesh.h>
#include <Geogram/GeogramBase.h>
#include <Geogram/GeogramMeshIO.h>
#include <Geogram/GeogramMeshUtils.h>

namespace py = pybind11;
using namespace PyMesh;

void init_Geogram(py::module &m) {
#if WITH_GEOGRAM
    GeogramBase init; // Initialize geogram once for all.
    m.def("load_geogram_mesh", [](const std::string& filename) {
            auto geo_mesh = GeogramMeshIO::load_mesh(filename);
            return GeogramMeshUtils::geomesh_to_mesh(geo_mesh);
            });
    m.def("save_geogram_mesh", [](const std::string& filename,
                Mesh::Ptr mesh) {
            auto geo_mesh = GeogramMeshUtils::mesh_to_geomesh(mesh);
            GeogramMeshIO::save_mesh(filename, geo_mesh);
            });
#endif
}
