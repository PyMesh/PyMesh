/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "Assembler2D.h"
#include "ZLazyPriorityQueue.h"
#include <iostream>
#include <map>

Assembler2D::Assembler2D() : m_D(3,3), m_C(3,3) {
    m_C << 1.0, 0.0, 0.0,
           0.0, 1.0, 0.0,
           0.0, 0.0, 2.0;
}

void Assembler2D::precomputeIsotropic(Mesh* m,
        double young_modulus, double poisson_ratio, double density) {
    m_mesh = m;
    m_E  = young_modulus;
    m_nu = poisson_ratio;
    m_density = density;

    m_lambda = (m_nu * m_E) / ((1.0 + m_nu) * (1.0 - 2.0 * m_nu));
    m_mu = m_E / (2.0 + 2.0 * m_nu);

    // Elastic modulii
    m_D << m_lambda + 2*m_mu,          m_lambda,    0.0,
           m_lambda         , m_lambda + 2*m_mu,    0.0,
           0.0              ,               0.0, 2*m_mu;

    precomputeShapeFunctionDerivatives();
}

void Assembler2D::precomputeOrthotropic(Mesh* m, double nu,
        double Ex, double Ey, double Ez,
        double G_yz, double G_zx, double G_xy,
        double density) {
    m_mesh = m;
    m_density = density;
    assert(false);
    return;
}

void Assembler2D::precomputeShapeFunctionDerivatives() {
    Eigen::MatrixXd selector = Eigen::MatrixXd::Zero(3,2);
    selector << 0.0, 0.0,
                1.0, 0.0,
                0.0, 1.0;

    m_DN.resize(m_mesh->getNbrElements());
    for (size_t i=0; i<m_mesh->getNbrElements(); ++i)
    {
        VectorI idx = m_mesh->getElement(i);
        assert(idx.size() == 3);
        VectorF u[3];
        u[0] = m_mesh->getNode(idx[0]);
        u[1] = m_mesh->getNode(idx[1]);
        u[2] = m_mesh->getNode(idx[2]);

        Eigen::MatrixXd P = Eigen::MatrixXd::Zero(3,3);
        P <<     1.0,     1.0,     1.0,
             u[0][0], u[1][0], u[2][0],
             u[0][1], u[1][1], u[2][1];

        // DN is a 4x3 matrix containing the gradients of the
        // 4 shape functions (one for each node)
        //
        m_DN[i] = P.inverse() * selector /* * -1.0 */;
    }
}

ZSparseMatrix Assembler2D::getMassMatrix(bool lumped, int repeats) {
    double p = m_density;

    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    if (lumped) {
        for (size_t i=0; i<m_mesh->getNbrElements(); i++) {
            VectorI idx = m_mesh->getElement(i);
            assert(idx.size() == 3);
            double V = m_mesh->getElementVolume(i);

            for (size_t j=0; j<idx.size(); j++)
                for (size_t l=0; l<repeats; l++)
                    triplets.push_back(T(repeats*idx[j]+l,
                                repeats*idx[j]+l, p*V/3.0));
        }
    } else {
        double coeff_jj = 1.0 / 6.0,
               coeff_jk = 1.0 / 12.0;
        for (size_t i=0; i<m_mesh->getNbrElements(); ++i) {
            VectorI idx = m_mesh->getElement(i);
            assert(idx.size() == 3);
            double V = m_mesh->getElementVolume(i);

            for (size_t j=0; j<3; ++j) {
                for (size_t k=0; k<3; ++k) {
                    if (idx[j] == idx[k]) {
                        for (size_t l=0; l<repeats; ++l)
                            triplets.push_back(
                                    T(repeats*idx[j]+l, repeats*idx[k]+l, p*V*coeff_jj));
                    } else {
                        for (size_t l=0; l<repeats; ++l)
                            triplets.push_back(
                                    T(repeats*idx[j]+l, repeats*idx[k]+l, p*V*coeff_jk));
                    }
                }
            }
        }
    }

    Eigen::SparseMatrix<double> M = Eigen::SparseMatrix<double>(
            repeats*m_mesh->getNbrNodes(), repeats*m_mesh->getNbrNodes());
    M.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(M);
}

ZSparseMatrix Assembler2D::getStiffnessMatrix() {
    // Elastic modulii
    //
    Eigen::MatrixXd& D = m_D;
    Eigen::MatrixXd& C = m_C;

    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    for (size_t i=0; i<m_mesh->getNbrElements(); ++i)
    {
        VectorI idx = m_mesh->getElement(i);
        assert(idx.size() == 3);

        Eigen::MatrixXd& dN = m_DN[i];

        // Small strain-displacement matrix
        //
        Eigen::MatrixXd B(3,6);
        B << dN(0,0),    0.0,dN(1,0),    0.0,dN(2,0),    0.0,
             0.0    ,dN(0,1),    0.0,dN(1,1),    0.0,dN(2,1),
             0.5*dN(0,1),0.5*dN(0,0),
             0.5*dN(1,1),0.5*dN(1,0),
             0.5*dN(2,1),0.5*dN(2,0);

        Eigen::MatrixXd k_el = B.transpose() * D * C * B * m_mesh->getElementVolume(i);

        for (size_t j=0; j<3; ++j)
            for (size_t k=0; k<3; ++k)
                for (size_t l=0; l<2; ++l)
                    for (size_t m=0; m<2; ++m)
                        triplets.push_back(T(2*idx[j]+l, 2*idx[k]+m, k_el(2*j+l, 2*k+m)));
    }

    Eigen::SparseMatrix<double> K = Eigen::SparseMatrix<double>(2*m_mesh->getNbrNodes(), 2*m_mesh->getNbrNodes());
    K.setFromTriplets(triplets.begin(), triplets.end());
    ZSparseMatrix tmp = ZSparseMatrix(K);
    return tmp;
}

ZSparseMatrix Assembler2D::getLaplacianMatrix() {
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    for (size_t i=0; i<m_mesh->getNbrElements(); ++i)
    {
        VectorI idx = m_mesh->getElement(i);
        assert(idx.size() == 3);

        Eigen::MatrixXd& dN = m_DN[i];

        // Small strain-displacement matrix
        //
        Eigen::MatrixXd B(2,3);
        B << dN(0,0), dN(1,0), dN(2,0),
             dN(0,1), dN(1,1), dN(2,1);

        Eigen::MatrixXd k_el = B.transpose() * B * m_mesh->getElementVolume(i);

        for (size_t j=0; j<3; ++j)
            for (size_t k=0; k<3; ++k)
                triplets.push_back(T(idx[j], idx[k], k_el(j,k)));
    }

    Eigen::SparseMatrix<double> L = Eigen::SparseMatrix<double>(m_mesh->getNbrNodes(), m_mesh->getNbrNodes());
    L.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(L);
}

ZSparseMatrix Assembler2D::getDisplacementStrainMatrix() {
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    for (size_t i=0; i<m_mesh->getNbrElements(); i++) {
        Eigen::MatrixXd dN = m_DN[i];
        VectorI idx = m_mesh->getElement(i);
        assert(idx.size() == 3);
        double V = m_mesh->getElementVolume(i);

        // e_xx
        size_t row = i * 3;
        for (size_t k=0; k<3; k++) {
            triplets.push_back(T(row, idx[k]*2, dN(k,0)));
        }

        // e_yy
        row++;
        for (size_t k=0; k<3; k++) {
            triplets.push_back(T(row, idx[k]*2+1, dN(k,1)));
        }

        // e_xy
        row++;
        for (size_t k=0; k<3; k++) {
            triplets.push_back(T(row, idx[k]*2  , dN(k,1) / 2.0));
            triplets.push_back(T(row, idx[k]*2+1, dN(k,0) / 2.0));
        }
    }

    Eigen::SparseMatrix<double> B = Eigen::SparseMatrix<double>(3*m_mesh->getNbrElements(), 2*m_mesh->getNbrNodes());
    B.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(B);
}

ZSparseMatrix Assembler2D::getStrainStressMatrix() {
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    for (size_t i=0; i<m_mesh->getNbrElements(); i++) {
        for (size_t j=0; j<3; j++) {
            for (size_t k=0; k<3; k++) {
                if (m_D(j,k) != 0) {
                    triplets.push_back(T(i*3+j, i*3+k, m_D(j,k)));
                }
            }
        }
    }

    Eigen::SparseMatrix<double> D = Eigen::SparseMatrix<double>(3*m_mesh->getNbrElements(), 3*m_mesh->getNbrElements());
    D.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(D);
}

ZSparseMatrix Assembler2D::getBdLaplacianMatrix() {
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    size_t num_bdv = m_mesh->getNbrBoundaryNodes();
    size_t num_bdf = m_mesh->getNbrBoundaryFaces();

    // Compute lumped mass
    VectorF lumped_mass(num_bdv);
    for (size_t i=0; i<num_bdv; i++) {
        VectorI neighbor_faces = m_mesh->getBoundaryNodeAdjacentBoundaryFaces(i);
        assert(neighbor_faces.size() == 2);

        double total_weight =
            m_mesh->getBoundaryFaceArea(neighbor_faces[0]) +
            m_mesh->getBoundaryFaceArea(neighbor_faces[1]);
        lumped_mass[i] = 0.5 * total_weight;
    }

    // Compute laplacian matrix.
    for (size_t i=0; i<num_bdf; i++) {
        VectorI face = m_mesh->getBoundaryFace(i);
        assert(face.size() == 2);

        double l = m_mesh->getBoundaryFaceArea(i);
        size_t v1 = m_mesh->getBoundaryIndex(face[0]);
        size_t v2 = m_mesh->getBoundaryIndex(face[1]);
        double weight = 1.0 / l;
        triplets.push_back(T(v1, v1, -weight / lumped_mass[v1]));
        triplets.push_back(T(v1, v2,  weight / lumped_mass[v1]));
        triplets.push_back(T(v2, v1,  weight / lumped_mass[v2]));
        triplets.push_back(T(v2, v2, -weight / lumped_mass[v2]));
    }

    Eigen::SparseMatrix<double> Lb = Eigen::SparseMatrix<double>(num_bdv, num_bdv);
    Lb.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(Lb);
}
