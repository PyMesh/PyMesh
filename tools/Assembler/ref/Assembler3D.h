/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "Assembler.h"
#include "AbstractMesh.h"

class Assembler3D : public Assembler {
    public:
        Assembler3D();

    public:
        size_t getDim() const { return 3; }

        void precomputeIsotropic(Mesh* m, double young_modulus, double poisson_ratio, double density);

        /**
         * Precompute the orthotropic material model.
         * Ex, Ey, Ez: Young's modulus
         * G_yz, G_zx, G_xy: Shear modulus
         *
         * If any of the shear modulus is negative, all shear modulus is
         * automatically computed from Young's modulus.
         */
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
        Eigen::MatrixXd m_C; // Corrector of the off diagonal elements of D.
};
