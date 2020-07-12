#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <Statistics/StatisticsEngine.h>
#include <Statistics/MeshStatistics.h>

namespace py = pybind11;
using namespace PyMesh;

void init_Statistics(py::module& m) {
    py::class_<StatisticsEngine, std::shared_ptr<StatisticsEngine> >(m, "StatisticsEngine")
        .def_static("create", &StatisticsEngine::create)
        .def("set_mesh", &StatisticsEngine::set_mesh)
        .def("get_vertices", &StatisticsEngine::get_vertices)
        .def("get_faces", &StatisticsEngine::get_faces)
        .def("compute_statistics", &StatisticsEngine::compute_statistics);

    py::class_<MeshStatistics, std::shared_ptr<MeshStatistics> > meshstatistics(m, "MeshStatistics");
    meshstatistics
        .def(py::init<>())
        .def_readwrite("nb_vertices", &MeshStatistics::nbVertices)
        .def_readwrite("nb_edges", &MeshStatistics::nbEdges)
        .def_readwrite("nb_facets", &MeshStatistics::nbFacets)
        .def_readwrite("min_edge_length", &MeshStatistics::minEdgeLength)
        .def_readwrite("max_edge_length", &MeshStatistics::maxEdgeLength)
        .def_readwrite("mean_edge_length", &MeshStatistics::meanEdgeLength)
        .def_readwrite("median_edge_length", &MeshStatistics::medianEdgeLength)
        .def_readwrite("nb_null_length_edge", &MeshStatistics::nbNullLengthEdge)

        .def_readwrite("min_angle", &MeshStatistics::minAngle)
        .def_readwrite("max_angle", &MeshStatistics::maxAngle)
        .def_readwrite("mean_angle", &MeshStatistics::meanAngle)

        .def_readwrite("min_area", &MeshStatistics::minArea)
        .def_readwrite("max_area", &MeshStatistics::maxArea)
        .def_readwrite("med_area", &MeshStatistics::medArea)
        .def_readwrite("mean_area", &MeshStatistics::meanArea)

        .def_readwrite("min_altitude", &MeshStatistics::minAltitude)
        .def_readwrite("min_aspect_ratio", &MeshStatistics::minAspectRatio)
        .def_readwrite("max_aspect_ratio", &MeshStatistics::maxAspectRatio)
        .def_readwrite("mean_aspect_ratio", &MeshStatistics::meanAspectRatio)

        .def_readwrite("num_degenerated_faces", &MeshStatistics::numDegeneratedFaces)
        .def_readwrite("area", &MeshStatistics::area)
        .def_readwrite("volume", &MeshStatistics::volume)
        .def_readwrite("self_intersect", &MeshStatistics::selfIntersect)
        .def_readwrite("genus", &MeshStatistics::genus)

        .def_readwrite("is_triangular_mesh", &MeshStatistics::isTriangularMesh)
        .def_readwrite("is_closed", &MeshStatistics::isClosed)
        .def_readwrite("has_non_manifold_vertices", &MeshStatistics::hasNonManifoldVertices)

        .def_readwrite("num_components", &MeshStatistics::numComponents)
        .def_readwrite("num_borders", &MeshStatistics::numBorders);
}
