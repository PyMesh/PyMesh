/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "Assembler.h"

#include <cmath>
#include <iostream>
#include <queue>
#include <vector>

#include <Mesh.h>

ZSparseMatrix Assembler::getPressureForceMatrix() {
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;
    size_t dim = getDim();

    for (size_t i=0; i<m_mesh->getNbrBoundaryNodes(); i++) {
        size_t glob_idx = m_mesh->getBoundaryNode(i);
        VectorF normal = m_mesh->getBoundaryNodeNormal(i);

        for (size_t j=0; j<dim; j++) {
            triplets.push_back(T(glob_idx*dim+j  , i, normal[j]));
        }
    }

    Eigen::SparseMatrix<double> P = Eigen::SparseMatrix<double>(
            dim*m_mesh->getNbrNodes(), m_mesh->getNbrBoundaryNodes());
    P.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(P);
}

ZSparseMatrix Assembler::getBdAreaMatrix() {
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;
    size_t dim = getDim();

    size_t num_bd_vertices = m_mesh->getNbrBoundaryNodes();
    for (size_t i=0; i<num_bd_vertices; i++) {
        VectorI bd_faces = m_mesh->getBoundaryNodeAdjacentBoundaryFaces(i);
        double area = 0.0;
        for (size_t j=0; j<bd_faces.size(); j++) {
            area += m_mesh->getBoundaryFaceArea(bd_faces[j]);
        }
        area /= double(dim);
        triplets.push_back(T(i, i, area));
    }

    Eigen::SparseMatrix<double> Pb = Eigen::SparseMatrix<double>(num_bd_vertices, num_bd_vertices);
    Pb.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(Pb);
}

// Item is basically a <key, value> pair.
typedef std::pair<int, double> Item;

/**
 * ItemComp is a functor for comparing Item objects based on value.
 * Item with smaller value will be placed in the front.
 */
struct ItemComp {
    bool operator() (const Item& a, const Item& b) const {
        return a.second > b.second;
    }
};

ZSparseMatrix Assembler::getBoundaryBlurMatrix(double radius) {
    typedef std::priority_queue<Item, std::vector<Item>, ItemComp> Queue;
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    size_t num_bd_vertices = m_mesh->getNbrBoundaryNodes();
    double sd = radius / 3.0;
    const double A = 2.50662827463; // A = sqrt(2*pi);

    std::vector<int> visited(num_bd_vertices, -1);

    for (size_t i=0; i<num_bd_vertices; i++) {
        size_t seed = i;
        Queue Q;
        Q.push(Item(i, 0.0));
        VectorF seed_n = m_mesh->getBoundaryNodeNormal(seed);

        while (!Q.empty()) {
            Item cur_item = Q.top();
            Q.pop();

            // Check if visited before
            if (visited[cur_item.first] == seed) continue;
            visited[cur_item.first] = seed;

            size_t curr_glob_idx = m_mesh->getBoundaryNode(cur_item.first);
            VectorF curr_n = m_mesh->getBoundaryNodeNormal(cur_item.first);
            double normal_factor = fmax(0.0, curr_n.dot(seed_n));

            // Use gaussian distribution as weights
            triplets.push_back(T(seed, cur_item.first,
                        exp(-0.5 * pow(cur_item.second / sd, 2)) /
                        (sd*A) * normal_factor));

            VectorI neighbors = m_mesh->getBoundaryNodeAdjacentBoundaryNodes(
                    cur_item.first);
            for (size_t j=0; j<neighbors.size(); j++) {
                size_t next = neighbors[j];
                if (visited[next] == seed) continue;

                size_t next_glob_idx = m_mesh->getBoundaryNode(next);
                double next_dist = cur_item.second +
                    (m_mesh->getNode(curr_glob_idx) -
                     m_mesh->getNode(next_glob_idx)).norm();
                if (next_dist > 2*sd) continue;
                Q.push(Item(next, next_dist));
            }
        }
    }

    Eigen::SparseMatrix<double> Bd_Blur = Eigen::SparseMatrix<double>(num_bd_vertices, num_bd_vertices);
    Bd_Blur.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(Bd_Blur);
}

VectorF Assembler::getDivergence(double* vector_field, int num_vector_field) {
    size_t num_node = m_mesh->getNbrNodes();
    size_t num_elem = m_mesh->getNbrElements();
    size_t dim = m_mesh->getDim();

    assert(num_vector_field == dim*num_node);

    VectorF div(num_elem);
    for (size_t i=0; i<num_elem; i++) {
        div[i] = 0;
        VectorI elem = m_mesh->getElement(i);
        const Eigen::MatrixXd& DN = m_DN[i];
        for (size_t j=0; j<elem.size(); j++) {
            for (size_t k=0; k<dim; k++) {
                div[i] += DN(j,k) * vector_field[elem[j]*dim+k];
            }
        }
    }
    return div;
}

VectorF Assembler::getCurlNorm(double* vector_field, int num_vector_field) {
    size_t num_node = m_mesh->getNbrNodes();
    size_t num_elem = m_mesh->getNbrElements();
    size_t dim = m_mesh->getDim();

    assert(num_vector_field == dim*num_node);

    VectorF curl_norm(num_elem);

    for (size_t i=0; i<num_elem; i++) {
        VectorI elem = m_mesh->getElement(i);
        const Eigen::MatrixXd& DN = m_DN[i];
        Eigen::Vector3d curl(0, 0, 0);
        for (size_t j=0; j<elem.size(); j++) {
            Eigen::Vector3d v(0, 0, 0);
            Eigen::Vector3d grad(0, 0, 0);
            for (size_t k=0; k<dim; k++) {
                v[k] = vector_field[elem[j]*dim+k];
                grad[k] = DN(j, k);
            }

            curl = curl + grad.cross(v);
        }
        curl_norm[i] = curl.norm();
    }
    return curl_norm;
}

ZSparseMatrix Assembler::getRigidMotionMatrix() {
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;
    size_t dim = m_mesh->getDim();

    // Translation part.
    for (size_t i=0; i<m_mesh->getNbrNodes(); i++) {
        for (size_t j=0; j<dim; j++) {
            triplets.push_back(T(j,i*dim+j, 1.0));
        }
    }

    // Compute centroid
    VectorF c(dim);
    c *= 0;

    // Rotation part.
    size_t rot_degrees = 0;
    if (dim == 2) {
        rot_degrees = 1;
        for (size_t i=0; i<m_mesh->getNbrNodes(); i++) {
            VectorF x = m_mesh->getNode(i);
            triplets.push_back(T(dim  , i*dim  , -x[1]+c[1]));
            triplets.push_back(T(dim  , i*dim+1,  x[0]-c[0]));
        }
    } else {
        rot_degrees = 3;
        for (size_t i=0; i<m_mesh->getNbrNodes(); i++) {
            VectorF x = m_mesh->getNode(i);
            triplets.push_back(T(dim  , i*dim+1, -x[2]+c[2]));
            triplets.push_back(T(dim  , i*dim+2,  x[1]-c[1]));
            triplets.push_back(T(dim+1, i*dim  ,  x[2]-c[2]));
            triplets.push_back(T(dim+1, i*dim+2, -x[0]+c[0]));
            triplets.push_back(T(dim+2, i*dim  , -x[1]+c[1]));
            triplets.push_back(T(dim+2, i*dim+1,  x[0]-c[0]));
        }
    }

    Eigen::SparseMatrix<double> Ru = Eigen::SparseMatrix<double>(dim + rot_degrees, dim * m_mesh->getNbrNodes());
    Ru.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(Ru);
}
