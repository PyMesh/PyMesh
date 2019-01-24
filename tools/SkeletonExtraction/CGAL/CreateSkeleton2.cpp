#include "CreateSkeleton2.h"

#include <list>
#include <vector>

#include<boost/shared_ptr.hpp>

#ifdef WITH_CGAL

#include<CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include<CGAL/Polygon_2.h>
#include<CGAL/create_straight_skeleton_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
typedef K::Point_2                   Point ;
typedef CGAL::Polygon_2<K>           Polygon_2;
typedef CGAL::Straight_skeleton_2<K> Ss ;
typedef boost::shared_ptr<Ss> SsPtr ;

using namespace PyMesh;

void CreateSkeleton2::run(const MatrixFr& points)
{
    Polygon_2 poly;
    std::size_t num_vertices = points.rows();

    for (size_t i=0; i<num_vertices; i++) {
        const VectorF& v = points.row(i);
        poly.push_back( Point(v[0],v[1]) );
    }

    std::vector<std::pair<Point,Point>> skeleton_edges;

    //compute skeleton
    SsPtr ss = CGAL::create_interior_straight_skeleton_2(poly.vertices_begin(), poly.vertices_end());

    typedef typename Ss::Halfedge_const_iterator Halfedge_const_iterator;
    for ( Halfedge_const_iterator i = ss->halfedges_begin(); i != ss->halfedges_end(); ++i )
    {
        CGAL::Point_2<K> pt1 = i->opposite()->vertex()->point();
        CGAL::Point_2<K> pt2 = i->vertex()->point();
        if( i->is_bisector() )
        {
            skeleton_edges.push_back(std::make_pair(Point(pt1.x(),pt1.y()),Point(pt2.x(),pt2.y())));
        }
    }

    size_t num_skeleton_edges = skeleton_edges.size();
    Matrix4Fr computed_edges(num_skeleton_edges,4); //this is the format that has to be returned
    for(size_t index = 0; index < num_skeleton_edges; ++index)
    {
        computed_edges(index,0) = skeleton_edges[index].first.x();
        computed_edges(index,1) = skeleton_edges[index].first.y();
        computed_edges(index,2) = skeleton_edges[index].second.x();
        computed_edges(index,3) = skeleton_edges[index].second.y();
    }

    m_edges = std::move(computed_edges);
}

#endif