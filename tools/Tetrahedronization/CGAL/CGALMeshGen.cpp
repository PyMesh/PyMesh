/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "CGALMeshGen.h"

#include <cassert>
#include <iostream>
#include <fstream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Polyhedral_mesh_domain_with_features_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>

using namespace PyMesh;

// Domain 
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedral_mesh_domain_with_features_3<K> Mesh_domain;
typedef CGAL::Polyhedron_3<K, CGAL::Mesh_3::Mesh_polyhedron_items<int> > Polyhedron;
typedef Polyhedron::HalfedgeDS HalfedgeDS;

// Triangulation
#ifdef CGAL_CONCURRENT_MESH_3
typedef CGAL::Mesh_triangulation_3<
    Mesh_domain,
    CGAL::Kernel_traits<Mesh_domain>::Kernel, // Same as sequential
    CGAL::Parallel_tag                        // Tag to activate parallelism
    >::type Tr;
#else
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
#endif

typedef CGAL::Mesh_complex_3_in_triangulation_3<
    Tr,
    Mesh_domain::Corner_index,
    Mesh_domain::Curve_segment_index> C3t3;

typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
using namespace CGAL::parameters;

namespace CGALMeshGenHelper {
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

    void extract_mesh(const C3t3& c3t3,
            MatrixFr& vertices, MatrixIr& faces, MatrixIr& voxels) {
        const Tr& tr = c3t3.triangulation();
        size_t num_vertices = tr.number_of_vertices();
        size_t num_faces = c3t3.number_of_facets_in_complex();
        size_t num_voxels = c3t3.number_of_cells_in_complex();

        vertices.resize(num_vertices, 3);
        faces.resize(num_faces, 3);
        voxels.resize(num_voxels, 4);

        std::map<Tr::Vertex_handle, int> V;
        size_t inum = 0;
        for(auto vit = tr.finite_vertices_begin();
                vit != tr.finite_vertices_end(); ++vit) {
            V[vit] = inum;
            const auto& p = vit->point();
            vertices.row(inum) = Vector3F(p.x(), p.y(), p.z()).transpose();
            assert(inum < num_vertices);
            inum++;
        }
        assert(inum == num_vertices);

        size_t face_count = 0;
        for(auto fit = c3t3.facets_in_complex_begin();
                fit != c3t3.facets_in_complex_end(); ++fit) {
            assert(face_count < num_faces);
            for (int i=0; i<3; i++) {
                if (i != fit->second) {
                    const auto& vh = (*fit).first->vertex(i);
                    assert(V.find(vh) != V.end());
                    const int vid = V[vh];
                    faces(face_count, i) = vid;
                }
            }
            face_count++;
        }
        assert(face_count == num_faces);

        size_t voxel_count = 0;
        for(auto cit = c3t3.cells_in_complex_begin() ;
                cit != c3t3.cells_in_complex_end(); ++cit ) {
            assert(voxel_count < num_voxels);
            for (int i=0; i<4; i++) {
                assert(V.find(cit->vertex(i)) != V.end());
                const size_t vid = V[cit->vertex(i)];
                voxels(voxel_count, i) = vid;
            }
            voxel_count++;
        }
        assert(voxel_count == num_voxels);
    }
}
using namespace CGALMeshGenHelper;

void CGALMeshGen::run() {
    preprocess();

    Polyhedron P = generate_polyhedron(m_vertices, m_faces);
    Mesh_domain domain(P);
    domain.detect_features();
    auto_compute_meshing_params();
    Mesh_criteria criteria(
            edge_size             =m_edge_size,
            facet_size            =m_face_size,
            cell_radius_edge_ratio=m_cell_radius_edge_ratio,
            cell_size             =m_cell_size);

    C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(
            domain, criteria, no_perturb(), no_exude());
    extract_mesh(c3t3, m_vertices, m_faces, m_voxels);
    std::ofstream medit_file("debug.yams2");
    c3t3.output_to_medit(medit_file);
}

