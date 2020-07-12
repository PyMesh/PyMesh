#ifndef PYMESH_CGALTYPES_H
#define PYMESH_CGALTYPES_H

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/array.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Nef_polyhedron_2.h>
#include <CGAL/Point_3.h>

typedef std::array<double, 3>                           Point_type;
typedef std::array<int, 3>                              Polygon_type;
typedef CGAL::Simple_cartesian<double>                  Kernel;
typedef CGAL::Polyhedron_3<Kernel>                      Polyhedron;

typedef Kernel::Point_3                                 Point;
typedef CGAL::Surface_mesh<Point>                       Mesh;

typedef std::array<double, 3>                           Point_type;
typedef std::array<int, 3>                              Polygon_type;
typedef CGAL::Simple_cartesian<double>                  Kernel;
typedef CGAL::Polyhedron_3<Kernel>                      Polyhedron;

typedef boost::graph_traits<Mesh>::face_descriptor      face_descriptor;
typedef boost::graph_traits<Mesh>::vertex_descriptor    vertex_descriptor;
typedef boost::graph_traits<Mesh>::halfedge_descriptor  halfedge_descriptor;
typedef boost::graph_traits<Mesh>::edge_descriptor      edge_descriptor;


#endif //PYMESH_CGALTYPES_H
