#ifndef CGAL_INCLUDES_H
#define CGAL_INCLUDES_H

//#ifdef INEXACT_CONSTRUCTION
// Use this for faster testing, but inexact meshing
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
//#else
//#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
//typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
//#endif

// Triangle triangle intersection
#include <CGAL/intersections.h>
// THIS CANNOT BE INCLUDED IN THE SAME FILE AS <CGAL/intersections.h>
// #include <CGAL/Boolean_set_operations_2.h>

// Constrained Delaunay Triangulation types
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_plus_2.h>

// Axis-align boxes for all-pairs self-intersection detection
#include <CGAL/point_generators_3.h>
#include <CGAL/Bbox_3.h>
#include <CGAL/box_intersection_d.h>
#include <CGAL/function_objects.h>
#include <CGAL/Join_input_iterator.h>
#include <CGAL/algorithm.h>
#include <vector>

// 3D Primitices
typedef CGAL::Point_3<Kernel>    Point_3;
typedef CGAL::Segment_3<Kernel>  Segment_3; 
typedef CGAL::Triangle_3<Kernel> Triangle_3; 
typedef CGAL::Plane_3<Kernel>    Plane_3;
typedef CGAL::Vector_3<Kernel>   Vector_3;

// 2D Primitives
typedef CGAL::Point_2<Kernel>    Point_2;
typedef CGAL::Segment_2<Kernel>  Segment_2; 
typedef CGAL::Triangle_2<Kernel> Triangle_2; 

// Constrained Delaunay Triangulation types
typedef CGAL::Triangulation_vertex_base_2<Kernel>  TVB_2;
typedef CGAL::Constrained_triangulation_face_base_2<Kernel> CTFB_2;
typedef CGAL::Triangulation_data_structure_2<TVB_2,CTFB_2> TDS_2;
typedef CGAL::Exact_intersections_tag Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<Kernel,TDS_2,Itag> CDT_2;
typedef CGAL::Constrained_triangulation_plus_2<CDT_2>       CDT_plus_2;

// Axis-align boxes for all-pairs self-intersection detection
typedef std::vector<Triangle_3> Triangles;
typedef Triangles::iterator TrianglesIterator;
typedef CGAL::Box_intersection_d::Box_with_handle_d<double,3,TrianglesIterator> Box;


#endif
