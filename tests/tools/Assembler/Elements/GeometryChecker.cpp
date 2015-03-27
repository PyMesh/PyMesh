/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "GeometryChecker.h"
#include <set>
#include <Misc/Triplet.h>

using namespace GeometryChecker;

namespace GeometryCheckerHelper {
    std::set<Triplet> extract_boundary(MeshPtr mesh) {
        std::set<Triplet> edge_set;
        const size_t vertex_per_face = mesh->get_vertex_per_face();
        size_t num_faces = mesh->get_num_faces();
        for (size_t i=0; i<num_faces; i++) {
            VectorI face = mesh->get_face(i);
            for (size_t j=0; j<vertex_per_face; j++) {
                Triplet edge(face[j], face[(j+1) % vertex_per_face]);
                std::set<Triplet>::const_iterator itr = edge_set.find(edge);
                if (itr == edge_set.end()) {
                    edge_set.insert(edge);
                } else {
                    edge_set.erase(itr);
                }
            }
        }
        return edge_set;
    }
}

void GeometryChecker::check_nodes(ElementsPtr m1, MeshPtr m2) {
    ASSERT_EQ(m1->getNbrNodes(), m2->get_num_vertices());
    ASSERT_EQ(m1->getNodes(), m2->get_vertices());
    const size_t num_nodes = m1->getNbrNodes();
    for (size_t i=0; i<num_nodes; i++) {
        ASSERT_EQ(m1->getNode(i), m2->get_vertex(i));
    }
}

void GeometryChecker::check_face_elements(ElementsPtr m1,
        MeshPtr m2) {
    ASSERT_EQ(m1->getNbrElements(), m2->get_num_faces());
    ASSERT_EQ(m1->getElements(), m2->get_faces());
    const size_t num_elements = m1->getNbrElements();
    for (size_t i=0; i<num_elements; i++) {
        ASSERT_EQ(m1->getElement(i), m2->get_face(i));
    }
}

void GeometryChecker::check_edge_elements(ElementsPtr m1,
        MeshPtr m2) {
    using namespace GeometryCheckerHelper;
    std::set<Triplet> edges = extract_boundary(m2);
    const size_t num_elements = m1->getNbrElements();
    ASSERT_EQ(edges.size(), num_elements);

    for (size_t i=0; i<num_elements; i++) {
        VectorI elem = m1->getElement(i);
        ASSERT_EQ(2, elem.size());
        Triplet edge(elem[0], elem[1]);
        ASSERT_TRUE(edges.find(edge) != edges.end());
    }
}

void GeometryChecker::check_voxel_elements(ElementsPtr m1,
        MeshPtr m2) {
    ASSERT_EQ(m1->getNbrElements(), m2->get_num_voxels());
    ASSERT_EQ(m1->getElements(), m2->get_voxels());
    const size_t num_elements = m1->getNbrElements();
    for (size_t i=0; i<num_elements; i++) {
        ASSERT_EQ(m1->getElement(i), m2->get_voxel(i));
    }
}
