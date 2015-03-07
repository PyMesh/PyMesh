#include "IGLSelfIntersectionResolver.h"

#include <igl/cgal/remesh_self_intersections.h>
#include <igl/remove_unreferenced.h>

void IGLSelfIntersectionResolver::run() {
    igl::RemeshSelfIntersectionsParam param;
    MatrixFr out_vertices;
    MatrixIr out_faces;
    MatrixIr intersecting_face_pairs;
    VectorI unique_vertex_indices;

    igl::remesh_self_intersections(
            m_vertices,
            m_faces,
            param,
            out_vertices,
            out_faces,
            intersecting_face_pairs,
            m_face_sources,
            unique_vertex_indices);

    std::for_each(out_faces.data(),out_faces.data()+out_faces.size(),
            [&unique_vertex_indices](int & a){
            a=unique_vertex_indices(a);
            });

    igl::remove_unreferenced(out_vertices,out_faces,
            m_vertices,m_faces,unique_vertex_indices);
}
