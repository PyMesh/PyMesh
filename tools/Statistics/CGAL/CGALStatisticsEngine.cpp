/* This file is part of PyMesh. Copyright (c) 2020 by Julien Jerphanion */
#ifdef WITH_CGAL
#include "CGALStatisticsEngine.h"

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include "CGALUtils.h"
#include "statistics_helpers.h"

using namespace PyMesh;

typedef std::array<double, 3>                           Point_type;
typedef std::array<int, 3>                              Polygon_type;
typedef CGAL::Simple_cartesian<double>                  Kernel;
typedef CGAL::Polyhedron_3<Kernel>                      Polyhedron;

typedef boost::graph_traits<Mesh>::face_descriptor      face_descriptor;
typedef boost::graph_traits<Mesh>::vertex_descriptor    vertex_descriptor;
typedef boost::graph_traits<Mesh>::halfedge_descriptor  halfedge_descriptor;
typedef boost::graph_traits<Mesh>::edge_descriptor      edge_descriptor;


void CGALStatisticsEngine::convert_mesh_to_native_format() {
    Polyhedron polyhedron = CGALUtils::mesh_to_polyhedron<Kernel>(m_vertices, m_faces);
    CGAL::copy_face_graph(polyhedron, m_mesh);
}

MeshStatistics CGALStatisticsEngine::compute_statistics() {

    MeshStatistics ms;

    ms.nbVertices = m_mesh.number_of_vertices();
    ms.nbEdges = m_mesh.number_of_halfedges() / 2;
    ms.nbFacets = m_mesh.number_of_faces();

    edges_length(&m_mesh, ms.minEdgeLength, ms.maxEdgeLength, ms.meanEdgeLength,
                 ms.medianEdgeLength, ms.nbNullLengthEdge);

    angles(&m_mesh, ms.minAngle, ms.maxAngle, ms.meanAngle);

    ms.isTriangularMesh = CGAL::is_triangle_mesh(m_mesh);

    if(ms.isTriangularMesh) {
        faces_area(&m_mesh, ms.minArea, ms.maxArea, ms.meanArea, ms.medArea);
        faces_aspect_ratio(&m_mesh, ms.minAltitude, ms.minAspectRatio, ms.maxAspectRatio, ms.meanAspectRatio);


        ms.numDegeneratedFaces = nb_degenerate_faces(&m_mesh);
        ms.area = CGAL::Polygon_mesh_processing::area(m_mesh);
        ms.selfIntersect = CGAL::Polygon_mesh_processing::does_self_intersect(m_mesh);

        ms.isClosed = CGAL::is_closed(m_mesh);

        if(ms.isClosed) {
            ms.volume = CGAL::Polygon_mesh_processing::volume(m_mesh);
            std::ptrdiff_t s(num_vertices(m_mesh)),
                    a(num_halfedges(m_mesh) / 2),
                    f(num_faces(m_mesh));

            ms.genus = 1.0 - double(s - a + f) / 2.0;
        }
    }

    ms.hasNonManifoldVertices = false;
    typedef boost::function_output_iterator<CGAL::internal::Throw_at_output> OutputIterator;
    try{
        CGAL::Polygon_mesh_processing::non_manifold_vertices(m_mesh, OutputIterator());
    }
    catch( CGAL::internal::Throw_at_output::Throw_at_output_exception& )
    {
        ms.hasNonManifoldVertices = true;
    }

    boost::vector_property_map<int,
            boost::property_map<Mesh, boost::face_index_t>::type>
            fccmap(get(boost::face_index, m_mesh));
    ms.numComponents = CGAL::Polygon_mesh_processing::connected_components(m_mesh, fccmap);
    ms.numBorders = 0;
    for(halfedge_descriptor hd : halfedges(m_mesh))
    {
        if(is_border(hd, m_mesh))
            ++ms.numBorders;
    }

    return ms;
}

#endif
