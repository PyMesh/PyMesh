/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once
#include <memory>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Implicit_mesh_domain_3.h>
#include <CGAL/Mesh_domain_with_polyline_features_3.h>
#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/Polyhedral_mesh_domain_with_features_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>

namespace PyMesh {

template<typename Domain>
class CGALMeshTraitsWithFeature {
    public:
#ifdef CGAL_CONCURRENT_MESH_3
        using Triangulation = typename CGAL::Mesh_triangulation_3<
            Domain,
            typename CGAL::Kernel_traits<Domain>::Kernel,
            typename CGAL::Parallel_tag>::type;
#else
        using Triangulation = typename CGAL::Mesh_triangulation_3<domain>::type;
#endif
        using C3t3 = CGAL::Mesh_complex_3_in_triangulation_3<
            Triangulation,
            typename Domain::Corner_index,
            typename Domain::Curve_segment_index>;
        using Criteria = CGAL::Mesh_criteria_3<Triangulation>;
};

template<typename Domain>
class CGALMeshTraits {
    public:
#ifdef CGAL_CONCURRENT_MESH_3
        using Triangulation = typename CGAL::Mesh_triangulation_3<
            Domain,
            typename CGAL::Kernel_traits<Domain>::Kernel,
            typename CGAL::Parallel_tag>::type;
#else
        using Triangulation = typename CGAL::Mesh_triangulation_3<domain>::type;
#endif
        using C3t3 = CGAL::Mesh_complex_3_in_triangulation_3<Triangulation>;
        using Criteria = CGAL::Mesh_criteria_3<Triangulation>;
};

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

template<typename K>
auto create_polyhedral_domain(const MatrixFr& vertices, const MatrixIr& faces) {
    using Polyhedron = CGAL::Polyhedron_3<K, CGAL::Mesh_3::Mesh_polyhedron_items<int> >;
    using Domain = CGAL::Polyhedral_mesh_domain_3<Polyhedron, K>;
    using HalfedgeDS = typename Polyhedron::HalfedgeDS;

    Polyhedron P;
    PolyhedronBuilder<HalfedgeDS> triangle(vertices, faces);
    P.delegate(triangle);
    assert(vertices.rows() == P.size_of_vertices());
    assert(faces.rows() == P.size_of_facets());

    return std::make_unique<Domain>(P);
}

template<typename K>
auto create_polyhedral_domain_with_feature(
        const MatrixFr& vertices, const MatrixIr& faces) {
    using Domain = CGAL::Polyhedral_mesh_domain_with_features_3<K>;
    using Polyhedron = CGAL::Polyhedron_3<K, CGAL::Mesh_3::Mesh_polyhedron_items<int> >;
    using HalfedgeDS = typename Polyhedron::HalfedgeDS;

    Polyhedron P;
    PolyhedronBuilder<HalfedgeDS> triangle(vertices, faces);
    P.delegate(triangle);
    assert(vertices.rows() == P.size_of_vertices());
    assert(faces.rows() == P.size_of_facets());

    auto domain = std::make_unique<Domain>(P);
    domain->detect_features();
    domain->detect_borders();
    return domain;
}

template<typename K, typename Oracle>
auto create_implicit_domain(
        const MatrixFr& vertices, const MatrixIr& faces, Oracle& oracle) {
    using Domain = CGAL::Implicit_mesh_domain_3<Oracle, K>;
    using Sphere = typename K::Sphere_3;
    using Point = typename K::Point_3;

	const size_t num_vertices = vertices.rows();
    assert(num_vertices > 0);
    Eigen::Vector3d centroid = vertices.colwise().sum() / num_vertices;
    Eigen::Vector3d bbox_min = vertices.colwise().minCoeff();
    Eigen::Vector3d bbox_max = vertices.colwise().maxCoeff();
    double bbox_radius = (bbox_max - bbox_min).norm() / 2;
    Sphere bounding_sphere(Point(centroid[0], centroid[1], centroid[2]), bbox_radius);
    return std::make_unique<Domain>(oracle, bounding_sphere);
}

template<typename K, typename Oracle>
auto create_implicit_domain_with_features(
        const MatrixFr& vertices, const MatrixIr& faces, Oracle& oracle) {
    using ImplicitDomain = CGAL::Implicit_mesh_domain_3<Oracle, K>;
    using Domain = CGAL::Mesh_domain_with_polyline_features_3<ImplicitDomain>;
    using Sphere = typename K::Sphere_3;
    using Point = typename K::Point_3;

	const size_t num_vertices = vertices.rows();
    assert(num_vertices > 0);
    Eigen::Vector3d centroid = vertices.colwise().sum() / num_vertices;
    Eigen::Vector3d bbox_min = vertices.colwise().minCoeff();
    Eigen::Vector3d bbox_max = vertices.colwise().maxCoeff();
    double bbox_radius = (bbox_max - bbox_min).norm() / 2;
    Sphere bounding_sphere(Point(centroid[0], centroid[1], centroid[2]), bbox_radius);
    auto domain  = std::make_unique<Domain>(oracle, bounding_sphere);
    // TODO: add polyline feature.
    return domain;
}

template<typename Traits>
void extract_mesh(const typename Traits::C3t3& c3t3,
        MatrixFr& vertices, MatrixIr& faces, MatrixIr& voxels) {
    const auto& tr = c3t3.triangulation();
    const size_t num_vertices = tr.number_of_vertices();
    const size_t num_faces = c3t3.number_of_facets_in_complex();
    const size_t num_voxels = c3t3.number_of_cells_in_complex();

    vertices.resize(num_vertices, 3);
    faces.resize(num_faces, 3);
    voxels.resize(num_voxels, 4);

    std::map<typename Traits::Triangulation::Vertex_handle, int> V;
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
        // CGAL provides no documentation on how to use `fit`.
        // Here is a guess from some reverse-engineering:
        // 
        //   `fit` is of type `std::pair<CellHandle, int>`.
        //
        // The first part gives a cell the facet belongs to.  The second
        // part tells us which vertex of the cell does NOT belong to the
        // facet (i.e. the opposite vertex).  The orientation of the facet
        // thus could be arbitrary.

        assert(fit->second < 4);
        assert(face_count < num_faces);
        auto vh1 = fit->first->vertex((fit->second+1)%4);
        auto vh2 = fit->first->vertex((fit->second+2)%4);
        auto vh3 = fit->first->vertex((fit->second+3)%4);

        assert(V.find(vh1) != V.end());
        assert(V.find(vh2) != V.end());
        assert(V.find(vh3) != V.end());

        faces.row(face_count) << V[vh1], V[vh2], V[vh3];
        assert((faces.row(face_count).array() >= 0).all());
        assert((faces.row(face_count).array() < num_vertices).all());
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

