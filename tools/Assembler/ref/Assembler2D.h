/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "Assembler.h"
#include "AbstractMesh.h"

class Assembler2D : public Assembler {
    public:
        Assembler2D();
    public:
        size_t getDim() const { return 2; }

        void precomputeIsotropic(Mesh* m, double young_modulus, double poisson_ratio, double density);
        void precomputeOrthotropic(Mesh* m, double nu,
                double Ex, double Ey, double Ez,
                double G_yz, double G_zx, double G_xy,
                double density);
        void precomputeShapeFunctionDerivatives();

        ZSparseMatrix getMassMatrix(bool lumped, int repeats);
        ZSparseMatrix getStiffnessMatrix();
        ZSparseMatrix getLaplacianMatrix();
        ZSparseMatrix getDisplacementStrainMatrix();
        ZSparseMatrix getStrainStressMatrix();
        ZSparseMatrix getBdLaplacianMatrix();

    private:
        Eigen::MatrixXd m_D;
        Eigen::MatrixXd m_C; // Corrector matrix
};
