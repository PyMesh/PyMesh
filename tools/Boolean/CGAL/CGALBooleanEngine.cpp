#include "CGALBooleanEngine.h"

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel>  Polyhedron;
typedef CGAL::Nef_polyhedron_3<Kernel> Nef_polyhedron;
typedef Polyhedron::HalfedgeDS HalfedgeDS;

namespace CGALBooleanEngineHelper {
template <class HDS>
    class PolyhedronBuilder : public CGAL::Modifier_base<HDS> {
        public:
            typedef typename HDS::Vertex Vertex;
            typedef typename Vertex::Point Point;

        public:
            PolyhedronBuilder(const MatrixFr& vertices, const MatrixIr& faces) :
                m_vertices(vertices), m_faces(faces) {}
            virtual ~PolyhedronBuilder() {}

        public:
            void operator()( HDS& hds) {
                CGAL::Polyhedron_incremental_builder_3<HDS> builder(hds, true);
                const size_t num_vertices = m_vertices.rows();
                const size_t num_faces = m_faces.rows();
                assert(3 == m_vertices.cols());
                assert(3 == m_faces.cols());

                builder.begin_surface(num_vertices, num_faces);

                for (size_t i=0; i<num_vertices; i++) {
                    const VectorF& v = m_vertices.row(i);
                    builder.add_vertex(Point(v[0], v[1], v[2]));
                }

                for (size_t i=0; i<num_faces; i++) {
                    const VectorI& f = m_faces.row(i);
                    builder.begin_facet();
                    builder.add_vertex_to_facet(f[0]);
                    builder.add_vertex_to_facet(f[1]);
                    builder.add_vertex_to_facet(f[2]);
                    builder.end_facet();
                }

                builder.end_surface();
            }

        private:
            MatrixFr m_vertices;
            MatrixIr m_faces;
    };

    Polyhedron generate_polyhedron(
            const MatrixFr& vertices, const MatrixIr& faces) {
        Polyhedron P;
        PolyhedronBuilder<HalfedgeDS> triangle(vertices, faces);
        P.delegate(triangle);
        assert(vertices.rows() == P.size_of_vertices());
        assert(faces.rows() == P.size_of_facets());
        return P;
    }

    void assert_mesh_is_closed(const Polyhedron& P) {
        if (!P.is_closed()) {
            throw RuntimeError("Mesh not closed.  Boolean requires closed mesh as input.");
        }
    }

    void extract_mesh(const Polyhedron& P,
            MatrixFr& vertices, MatrixIr& faces) {
        if (!P.is_pure_triangle()) {
            throw RuntimeError("CGAL boolean output non-triangulare mesh!");
        }

        const size_t dim = 3, vertex_per_face = 3;
        const size_t num_vertices = P.size_of_vertices();
        const size_t num_faces = P.size_of_facets();
        vertices.resize(num_vertices, dim);
        faces.resize(num_faces, vertex_per_face);

        size_t v_count = 0;
        for (auto v_itr=P.points_begin(); v_itr != P.points_end(); v_itr++) {
            vertices.row(v_count) = Vector3F(
                    CGAL::to_double(v_itr->x()),
                    CGAL::to_double(v_itr->y()),
                    CGAL::to_double(v_itr->z()));
            v_count++;
        }
        size_t f_count = 0;
        for (auto f_itr=P.facets_begin(); f_itr != P.facets_end(); f_itr++) {
            auto f_circulator = f_itr->facet_begin();
            size_t j=0;
            do {
                assert(j<vertex_per_face);
                faces(f_count, j) =
                    std::distance(P.vertices_begin(), f_circulator->vertex());
                f_circulator++;
                j++;
            } while (f_circulator != f_itr->facet_begin());
            f_count++;
        }
    }
}

using namespace CGALBooleanEngineHelper;

void CGALBooleanEngine::compute_union() {
    Polyhedron mesh1 = generate_polyhedron(m_vertices_1, m_faces_1);
    Polyhedron mesh2 = generate_polyhedron(m_vertices_2, m_faces_2);
    assert_mesh_is_closed(mesh1);
    assert_mesh_is_closed(mesh2);
    Nef_polyhedron nef_mesh1(mesh1);
    Nef_polyhedron nef_mesh2(mesh2);
    Nef_polyhedron nef_result = nef_mesh1 + nef_mesh2;
    if (nef_result.is_simple()) {
        Polyhedron result;
        nef_result.convert_to_polyhedron(result);
        extract_mesh(result, m_vertices, m_faces);
    } else {
        throw RuntimeError("Boolean result is not a simple polyhedron!");
    }
}

void CGALBooleanEngine::compute_intersection() {
    Polyhedron mesh1 = generate_polyhedron(m_vertices_1, m_faces_1);
    Polyhedron mesh2 = generate_polyhedron(m_vertices_2, m_faces_2);
    assert_mesh_is_closed(mesh1);
    assert_mesh_is_closed(mesh2);
    Nef_polyhedron nef_mesh1(mesh1);
    Nef_polyhedron nef_mesh2(mesh2);
    Nef_polyhedron nef_result = nef_mesh1 * nef_mesh2;
    if (nef_result.is_simple()) {
        Polyhedron result;
        nef_result.convert_to_polyhedron(result);
        extract_mesh(result, m_vertices, m_faces);
    } else {
        throw RuntimeError("Boolean result is not a simple polyhedron!");
    }
}

void CGALBooleanEngine::compute_difference() {
    Polyhedron mesh1 = generate_polyhedron(m_vertices_1, m_faces_1);
    Polyhedron mesh2 = generate_polyhedron(m_vertices_2, m_faces_2);
    assert_mesh_is_closed(mesh1);
    assert_mesh_is_closed(mesh2);
    Nef_polyhedron nef_mesh1(mesh1);
    Nef_polyhedron nef_mesh2(mesh2);
    Nef_polyhedron nef_result = nef_mesh1 - nef_mesh2;
    if (nef_result.is_simple()) {
        Polyhedron result;
        nef_result.convert_to_polyhedron(result);
        extract_mesh(result, m_vertices, m_faces);
    } else {
        throw RuntimeError("Boolean result is not a simple polyhedron!");
    }
}

void CGALBooleanEngine::compute_symmetric_difference() {
    Polyhedron mesh1 = generate_polyhedron(m_vertices_1, m_faces_1);
    Polyhedron mesh2 = generate_polyhedron(m_vertices_2, m_faces_2);
    assert_mesh_is_closed(mesh1);
    assert_mesh_is_closed(mesh2);
    Nef_polyhedron nef_mesh1(mesh1);
    Nef_polyhedron nef_mesh2(mesh2);
    Nef_polyhedron nef_result = nef_mesh1 ^ nef_mesh2;
    if (nef_result.is_simple()) {
        Polyhedron result;
        nef_result.convert_to_polyhedron(result);
        extract_mesh(result, m_vertices, m_faces);
    } else {
        throw RuntimeError("Boolean result is not a simple polyhedron!");
    }
}

