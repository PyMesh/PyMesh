/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "Assembler3D.h"
#include "ZLazyPriorityQueue.h"
#include "timing.h"
#include <iostream>
#include <fstream>
#include <map>

Assembler3D::Assembler3D() : m_D(6,6), m_C(6,6){
    // m_C double the off-diagonal components of the stress tensor.
    m_C << 1.0, 0.0, 0.0, 0.0, 0.0, 0.0,
           0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
           0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
           0.0, 0.0, 0.0, 2.0, 0.0, 0.0,
           0.0, 0.0, 0.0, 0.0, 2.0, 0.0,
           0.0, 0.0, 0.0, 0.0, 0.0, 2.0;
}

void Assembler3D::precomputeIsotropic(Mesh *m,
        double young_modulus,
        double poisson_ratio,
        double density) {
    timestamp_type start, end;
    get_timestamp(&start);

    m_mesh = m;
    m_E  = young_modulus;
    m_nu = poisson_ratio;
    m_density = density;

    m_lambda = (m_nu * m_E) / ((1.0 + m_nu) * (1.0 - 2.0 * m_nu));
    m_mu = m_E / (2.0 + 2.0 * m_nu);

    // Elastic modulii
    m_D << m_lambda + 2*m_mu,  m_lambda         ,  m_lambda         ,  0.0   ,  0.0   ,  0.0   ,
           m_lambda         ,  m_lambda + 2*m_mu,  m_lambda         ,  0.0   ,  0.0   ,  0.0   ,
           m_lambda         ,  m_lambda         ,  m_lambda + 2*m_mu,  0.0   ,  0.0   ,  0.0   ,
           0.0              ,                0.0,                0.0,  2*m_mu,  0.0   ,  0.0   ,
           0.0              ,                0.0,                0.0,  0.0   ,  2*m_mu,  0.0   ,
           0.0              ,                0.0,                0.0,  0.0   ,  0.0   ,  2*m_mu;

    precomputeShapeFunctionDerivatives();

    get_timestamp(&end);
    double duration = timestamp_diff_in_seconds(start, end);
    std::cout << "precompute_isotropic: " << duration << "s" << std::endl;
}

void Assembler3D::precomputeOrthotropic(Mesh* m, double nu,
        double Ex, double Ey, double Ez,
        double G_yz, double G_zx, double G_xy,
        double density) {
    timestamp_type start, end;
    get_timestamp(&start);

    m_mesh = m;
    m_density = density;
    double v_xy = 2 * nu * Ex / (Ex + Ey);
    double v_yx = 2 * nu * Ey / (Ex + Ey);
    double v_xz = 2 * nu * Ex / (Ex + Ez);
    double v_zx = 2 * nu * Ez / (Ex + Ez);
    double v_yz = 2 * nu * Ey / (Ey + Ez);
    double v_zy = 2 * nu * Ez / (Ey + Ez);
    if (G_xy <= 0 || G_yz <=0 || G_zx <= 0) {
        // Gauss shear modulus
        // Formula from:
        // http://help.solidworks.com/2011/english/SolidWorks/cworks/LegacyHelp/Simulation/Materials/Material_models/Linear_Elastic_Orthotropic_Model.htm
        G_xy = Ex*Ey / (Ex+Ey+2*Ey*v_xy);
        G_yz = Ey*Ez / (Ey+Ez+2*Ez*v_yz);
        G_zx = Ez*Ex / (Ez+Ex+2*Ex*v_zx);
    }
    // Elastic modulii
    Eigen::MatrixXd D_inv(6,6);
    D_inv << 1.0/Ex  ,  -v_yx/Ey,  -v_zx/Ez,  0.0     ,  0.0     ,  0.0     ,
             -v_xy/Ex,    1.0/Ey,  -v_zy/Ez,  0.0     ,  0.0     ,  0.0     ,
             -v_xz/Ex,  -v_yz/Ey,    1.0/Ez,  0.0     ,  0.0     ,  0.0     ,
             0.0     ,       0.0,       0.0,  1.0/G_xy,  0.0     ,  0.0     ,
             0.0     ,       0.0,       0.0,  0.0     ,  1.0/G_zx,  0.0     ,
             0.0     ,       0.0,       0.0,  0.0     ,  0.0     ,  1.0/G_yz;
    m_D = D_inv.inverse();
    precomputeShapeFunctionDerivatives();

    get_timestamp(&end);
    double duration = timestamp_diff_in_seconds(start, end);
    std::cout << "precompute_orthotropic: " << duration << "s" << std::endl;
}

void Assembler3D::precomputeShapeFunctionDerivatives() {
    Eigen::MatrixXd selector = Eigen::MatrixXd::Zero(4,3);
    selector << 0.0, 0.0, 0.0,
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0;

    m_DN.resize(m_mesh->getNbrElements());
    for (size_t i=0; i<m_mesh->getNbrElements(); ++i)
    {
        VectorI idx = m_mesh->getElement(i);
        assert(idx.size() == 4);
        VectorF u[4];
        u[0] = m_mesh->getNode(idx[0]);
        u[1] = m_mesh->getNode(idx[1]);
        u[2] = m_mesh->getNode(idx[2]);
        u[3] = m_mesh->getNode(idx[3]);

        Eigen::MatrixXd P = Eigen::MatrixXd::Zero(4,4);
        P <<     1.0,     1.0,     1.0,     1.0,
             u[0][0], u[1][0], u[2][0], u[3][0],
             u[0][1], u[1][1], u[2][1], u[3][1],
             u[0][2], u[1][2], u[2][2], u[3][2];

        // DN is a 4x3 matrix containing the gradients of the
        // 4 shape functions (one for each node)
        //
        m_DN[i] = P.inverse() * selector /* * -1.0 */;
    }
}

ZSparseMatrix Assembler3D::getMassMatrix(bool lumped, int repeats) {
    double p = m_density;

    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    if (lumped) {
        for (size_t i=0; i<m_mesh->getNbrElements(); i++) {
            VectorI idx = m_mesh->getElement(i);
            assert(idx.size() == 4);
            double V = m_mesh->getElementVolume(i);

            for (size_t j=0; j<idx.size(); j++)
                for (size_t l=0; l<repeats; l++)
                    triplets.push_back(T(repeats*idx[j]+l,
                                repeats*idx[j]+l, p*V/4.0));
        }
    } else {
        double coeff_jj = 0.1,
               coeff_jk = 0.05;
        for (size_t i=0; i<m_mesh->getNbrElements(); ++i) {
            VectorI idx = m_mesh->getElement(i);
            assert(idx.size() == 4);
            double V = m_mesh->getElementVolume(i);

            for (size_t j=0; j<4; ++j) {
                for (size_t k=0; k<4; ++k) {
                    if (idx[j] == idx[k]) {
                        for (size_t l=0; l<repeats; ++l)
                            triplets.push_back(
                                    T(repeats*idx[j]+l,
                                        repeats*idx[k]+l, p*V*coeff_jj));
                    } else {
                        for (size_t l=0; l<repeats; ++l)
                            triplets.push_back(
                                    T(repeats*idx[j]+l,
                                        repeats*idx[k]+l, p*V*coeff_jk));
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

ZSparseMatrix Assembler3D::getStiffnessMatrix() {
    // Elastic modulii
    Eigen::MatrixXd& D = m_D;
    Eigen::MatrixXd& C = m_C;

    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    for (size_t i=0; i<m_mesh->getNbrElements(); ++i)
    {
        VectorI idx = m_mesh->getElement(i);
        assert(idx.size() == 4);

        Eigen::MatrixXd &dN = m_DN[i];

        // Small strain-displacement matrix
        //
        Eigen::MatrixXd B(6,12);
        B << dN(0,0),0.0,0.0,dN(1,0),0.0,0.0,dN(2,0),0.0,0.0,dN(3,0),0.0,0.0,
             0.0,dN(0,1),0.0,0.0,dN(1,1),0.0,0.0,dN(2,1),0.0,0.0,dN(3,1),0.0,
             0.0,0.0,dN(0,2),0.0,0.0,dN(1,2),0.0,0.0,dN(2,2),0.0,0.0,dN(3,2),

             0.5*dN(0,1),0.5*dN(0,0),0.0,
             0.5*dN(1,1),0.5*dN(1,0),0.0,
             0.5*dN(2,1),0.5*dN(2,0),0.0,
             0.5*dN(3,1),0.5*dN(3,0),0.0,

             0.5*dN(0,2),0.0,0.5*dN(0,0),
             0.5*dN(1,2),0.0,0.5*dN(1,0),
             0.5*dN(2,2),0.0,0.5*dN(2,0),
             0.5*dN(3,2),0.0,0.5*dN(3,0),

             0.0,0.5*dN(0,2),0.5*dN(0,1),
             0.0,0.5*dN(1,2),0.5*dN(1,1),
             0.0,0.5*dN(2,2),0.5*dN(2,1),
             0.0,0.5*dN(3,2),0.5*dN(3,1);


        Eigen::MatrixXd k_el = B.transpose() * D * C * B * m_mesh->getElementVolume(i);

        for (size_t j=0; j<4; ++j)
            for (size_t k=0; k<4; ++k)
                for (size_t l=0; l<3; ++l)
                    for (size_t m=0; m<3; ++m)
                        triplets.push_back(T(3*idx[j]+l, 3*idx[k]+m, k_el(3*j+l, 3*k+m)));
    }

    Eigen::SparseMatrix<double> K = Eigen::SparseMatrix<double>(3*m_mesh->getNbrNodes(),3*m_mesh->getNbrNodes());
    K.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(K);
}

ZSparseMatrix Assembler3D::getLaplacianMatrix() {
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    for (size_t i=0; i<m_mesh->getNbrElements(); ++i)
    {
        VectorI idx = m_mesh->getElement(i);
        assert(idx.size() == 4);

        Eigen::MatrixXd &dN = m_DN[i];

        // Small strain-displacement matrix
        // B = grad u for approximation.
        Eigen::MatrixXd B(3,4);
        B << dN(0,0),dN(1,0), dN(2,0),dN(3,0),
             dN(0,1),dN(1,1), dN(2,1),dN(3,1),
             dN(0,2),dN(1,2), dN(2,2),dN(3,2);

        Eigen::MatrixXd k_el = B.transpose() * B * m_mesh->getElementVolume(i);

        for (size_t j=0; j<4; ++j)
            for (size_t k=0; k<4; ++k)
                triplets.push_back(T(idx[j],idx[k], k_el(j,k)));
    }

    Eigen::SparseMatrix<double> L = Eigen::SparseMatrix<double>(m_mesh->getNbrNodes(),m_mesh->getNbrNodes());
    L.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(L);
}

ZSparseMatrix Assembler3D::getDisplacementStrainMatrix() {
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    for (size_t i=0; i<m_mesh->getNbrElements(); i++) {
        Eigen::MatrixXd dN = m_DN[i];
        VectorI idx = m_mesh->getElement(i);
        double V = m_mesh->getElementVolume(i);

        // e_xx
        size_t row = i * 6;
        for (size_t k=0; k<4; k++) {
            triplets.push_back(T(row, idx[k]*3, dN(k,0)));
        }

        // e_yy
        row++;
        for (size_t k=0; k<4; k++) {
            triplets.push_back(T(row, idx[k]*3+1, dN(k,1)));
        }

        // e_zz
        row++;
        for (size_t k=0; k<4; k++) {
            triplets.push_back(T(row, idx[k]*3+2, dN(k,2)));
        }

        // e_xy
        row++;
        for (size_t k=0; k<4; k++) {
            triplets.push_back(T(row, idx[k]*3  , dN(k,1) / 2.0));
            triplets.push_back(T(row, idx[k]*3+1, dN(k,0) / 2.0));
        }

        // e_xz
        row++;
        for (size_t k=0; k<4; k++) {
            triplets.push_back(T(row, idx[k]*3  , dN(k,2) / 2.0));
            triplets.push_back(T(row, idx[k]*3+2, dN(k,0) / 2.0));
        }

        // e_yz
        row++;
        for (size_t k=0; k<4; k++) {
            triplets.push_back(T(row, idx[k]*3+1, dN(k,2) / 2.0));
            triplets.push_back(T(row, idx[k]*3+2, dN(k,1) / 2.0));
        }
    }

    Eigen::SparseMatrix<double> B = Eigen::SparseMatrix<double>(6*m_mesh->getNbrElements(), 3*m_mesh->getNbrNodes());
    B.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(B);
}

ZSparseMatrix Assembler3D::getStrainStressMatrix() {
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    for (size_t i=0; i<m_mesh->getNbrElements(); i++) {
        for (size_t j=0; j<6; j++) {
            for (size_t k=0; k<6; k++) {
                if (m_D(j,k) != 0) {
                    triplets.push_back(T(i*6+j, i*6+k, m_D(j,k)));
                }
            }
        }
    }

    Eigen::SparseMatrix<double> D = Eigen::SparseMatrix<double>(6*m_mesh->getNbrElements(), 6*m_mesh->getNbrElements());
    D.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(D);
}

ZSparseMatrix Assembler3D::getBdLaplacianMatrix() {
    typedef Eigen::Triplet<double> T;
    std::vector<T> triplets;

    size_t num_bdv = m_mesh->getNbrBoundaryNodes();
    size_t num_bdf = m_mesh->getNbrBoundaryFaces();

    // Compute lumped mass
    VectorF lumped_mass(num_bdv);
    for (size_t i=0; i<num_bdv; i++) {
        VectorI neighbor_faces = m_mesh->getBoundaryNodeAdjacentBoundaryFaces(i);
        assert(neighbor_faces.size() == 3);

        double total_weight =
            m_mesh->getBoundaryFaceArea(neighbor_faces[0]) +
            m_mesh->getBoundaryFaceArea(neighbor_faces[1]) +
            m_mesh->getBoundaryFaceArea(neighbor_faces[2]);
        lumped_mass[i] = total_weight / 3.0;
    }

    // Compute laplacian matrix.
    for (size_t i=0; i<num_bdf; i++) {
        VectorI face = m_mesh->getBoundaryFace(i);
        assert(face.size() == 3);

        size_t v[3] = {
            m_mesh->getBoundaryIndex(face[0]),
            m_mesh->getBoundaryIndex(face[1]),
            m_mesh->getBoundaryIndex(face[2])
        };
        Eigen::Vector3d p[3] = {
            Eigen::Vector3d(m_mesh->getNode(face[0])),
            Eigen::Vector3d(m_mesh->getNode(face[1])),
            Eigen::Vector3d(m_mesh->getNode(face[2]))
        };

        for (size_t j=0; j<3; j++) {
            size_t i0 = j;
            size_t i1 = (j+1)%3;
            size_t i2 = (j+2)%3;

            double cos_angle = (p[i1] - p[i0]).dot(p[i2] - p[i0]);
            double sin_angle = ((p[i1] - p[i0]).cross(p[i2] - p[i0])).norm();
            double weight = 0.5 * cos_angle / sin_angle;

            triplets.push_back(T(v[i1], v[i1], -weight / lumped_mass[v[i1]]));
            triplets.push_back(T(v[i1], v[i2],  weight / lumped_mass[v[i1]]));
            triplets.push_back(T(v[i2], v[i1],  weight / lumped_mass[v[i2]]));
            triplets.push_back(T(v[i2], v[i2], -weight / lumped_mass[v[i2]]));
        }
    }

    Eigen::SparseMatrix<double> Lb = Eigen::SparseMatrix<double>(num_bdv, num_bdv);
    Lb.setFromTriplets(triplets.begin(), triplets.end());
    return ZSparseMatrix(Lb);
}

