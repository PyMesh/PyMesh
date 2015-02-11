#include "CarveEngine.h"

#include <cassert>
#include <vector>
#include <carve/poly.hpp>

namespace CarveEngineHelper {
    carve::poly::Polyhedron create_polyhedron(
            const MatrixFr& vertices, const MatrixIr& faces) {
        typedef carve::poly::Vertex CarveVertex;
        typedef carve::poly::Face   CarveFace;
        typedef carve::geom::VECTOR CarveVector;

        const size_t num_vertices = vertices.rows();
        const size_t num_faces = faces.rows();
        assert(vertices.cols() == 3);
        assert(faces.cols() == 3);

        std::vector<CarveVertex> carve_vertices;
        std::vector<CarveFace *> carve_faces;

        for (size_t i=0; i<num_vertices; i++) {
            const auto& v = vertices.row(i);
            carve_vertices.push_back(
                    CarveVertex(CarveVector(v[0], v[1], v[2])));
        }

        for (size_t i=0; i<num_faces; i++) {
            const auto& f = faces.row(i);
            carve_faces.push_back(
                    new CarveFace(
                        &carve_vertices[f[0]],
                        &carve_vertices[f[1]],
                        &carve_vertices[f[2]]));
        }

        return carve::poly::Polyhedron(faces, vertices);
    }
}
using namespace CarveEngineHelper;

void CarveEngine::compute_union() {
}

void CarveEngine::compute_intersection() {
}

void CarveEngine::compute_difference() {
}

void CarveEngine::compute_symmetric_difference() {
}
