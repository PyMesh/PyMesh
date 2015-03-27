/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
/**
 * Assembler class is used to assemble various matrices required.
 */

#include <Eigen/Sparse>
#include <vector>

#include <Mesh.h>

#include "ZSparseMatrix.h"

class Assembler {
    public:
        virtual ~Assembler() {}

        virtual void precomputeIsotropic(Mesh *m,
                double young_modulus,
                double poisson_ratio,
                double density)=0;

        virtual void precomputeOrthotropic(Mesh *m, double poisson_ratio,
                double young_modulus_x, double young_modulus_y, double young_modulus_z,
                double shear_modulus_yz, double shear_modulus_zx, double shear_modulus_xy,
                double density)=0;

    public:
        // Abstract methods
        // Dimention/material dependent matrix assemblies.
        virtual size_t getDim() const=0;
        virtual void precomputeShapeFunctionDerivatives()=0;

        /**
         * Assemble the mass matrix.
         * @param[in]  lumped:  Lumped mass, mass concentrated at vertices.
         * @param[in]  repeats: number of times to repeat each mass matrix
         *                      entry.  For example,
         *
         *                      repeats=1  [m_ij]
         *
         *                      repeats=3  [m_ij    0    0]
         *                                 [   0 m_ij    0]
         *                                 [   0    0 m_ij]
         */
        virtual ZSparseMatrix getMassMatrix(bool lumped, int repeats)=0;
        virtual ZSparseMatrix getStiffnessMatrix()=0;
        virtual ZSparseMatrix getLaplacianMatrix()=0;
        virtual ZSparseMatrix getDisplacementStrainMatrix()=0;
        virtual ZSparseMatrix getStrainStressMatrix()=0;
        virtual ZSparseMatrix getBdLaplacianMatrix()=0;

    public:
        // Geometry dependent matrix assemblies
        ZSparseMatrix getPressureForceMatrix();
        ZSparseMatrix getBdAreaMatrix();
        ZSparseMatrix getBoundaryBlurMatrix(double radius);
        ZSparseMatrix getRigidMotionMatrix();

    public:
        // Other functions.
        // There functions process the input field without generating a matrix.

        /**
         * Compute the divergence of the given vector field.
         * n: number of nodes  m: number of elements
         *
         * @param[in] vector_field  size n*dim
         * @return                  size m
         */
        VectorF getDivergence(double* vector_field, int num_vector_field);

        /**
         * Compute the curl norm of the given vector field.
         * n: number of nodes  m: number of elements
         *
         * @param[in] vector_field  size n*dim
         * @return                  size m
         */
        VectorF getCurlNorm(double* vector_field, int num_vector_field);

    protected:
        // Material related parameters
        double m_E;
        double m_nu;
        double m_lambda;
        double m_mu;
        double m_beta;
        double m_density;

        // Shape function derivatives.
        std::vector<Eigen::MatrixXd> m_DN;

        Mesh* m_mesh;
};
