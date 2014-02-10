#!/usr/bin/env python

import numpy as np
import os
import os.path
import scipy.sparse
from scipy.io import loadmat
import sys
import unittest

PYMESH_PATH = os.environ.get("PYMESH_PATH");
if PYMESH_PATH is None:
    raise ImportError("Please set PYMESH_PATH to the correct lib path.")
sys.path.append(os.path.join(PYMESH_PATH, "lib"));
sys.path.append(os.path.join(PYMESH_PATH, "swig"));

import PyMesh
import PyAssembler

class FEAssemblerTest(unittest.TestCase):
    def setUp(self):
        self.data_dir = os.path.join(PYMESH_PATH, "tests/data/");
        self.ground_truth_dir = os.path.join(PYMESH_PATH,
                "tests/tools/Assembler/ground_truth/");
        self.material_name = "test_material";

    def load_mesh(self, mesh_file):
        mesh_file = os.path.join(self.data_dir, mesh_file);
        if not os.path.exists(mesh_file):
            raise IOError("mesh file {} does not exist!".format(mesh_file));

        factory = PyMesh.MeshFactory();
        factory.load_file(mesh_file);
        factory.with_connectivity("all");
        factory.with_attribute("face_normal");
        factory.with_attribute("vertex_normal");
        factory.with_attribute("face_area");
        factory.with_attribute("voxel_volume");
        return factory.create_shared();

    def create_assembler(self, mesh):
        return PyAssembler.FEAssembler.create_from_name(mesh, self.material_name);

    def create_assembler_from_material(self, mesh):
        material = PyAssembler.Material.create_isotropic(
                mesh.get_dim(), 1.0, 1.0, 0.0);
        return PyAssembler.FEAssembler.create(mesh, material);

    def load_matrix(self, filename, matrix_name):
        filename = os.path.join(self.ground_truth_dir, filename);
        matrix = loadmat(filename)[matrix_name];
        return matrix;

    def format(self, matrix):
        if isinstance(matrix, PyAssembler.ZSparseMatrix):
            rows = matrix.num_rows();
            cols = matrix.num_cols();

            inner_indices = np.ndarray(matrix.get_inner_size(), dtype=np.int32);
            outer_indices = np.ndarray(matrix.get_outer_size(), dtype=np.int32);
            data          = np.ndarray(matrix.get_value_size(), dtype=float);

            matrix.get_inner_indices(inner_indices);
            matrix.get_outer_indices(outer_indices);
            matrix.get_values(data);

            return scipy.sparse.csc_matrix((data, inner_indices, outer_indices),
                    shape=(rows, cols), dtype=float);

        else:
            raise NotImplementedError("Unknown matrix format: {}".format(
                str(type(matrix))));

    def assertMatrixEqual(self, M1, M2):
        diff = M1 - M2;
        self.assertAlmostEqual(0.0, diff.max());
        self.assertAlmostEqual(0.0, diff.min());

    def test_creation(self):
        mesh = self.load_mesh("tet.msh");
        assembler_1 = self.create_assembler(mesh);
        assembler_2 = self.create_assembler_from_material(mesh);
        K1 = self.format(assembler_1.assemble("stiffness"));
        K2 = self.format(assembler_2.assemble("stiffness"));
        self.assertMatrixEqual(K1, K2);

    def test_stiffness_tet(self):
        mesh = self.load_mesh("tet.msh");
        assembler = self.create_assembler(mesh);
        K = self.format(assembler.assemble("stiffness"));
        K_ground_truth = self.load_matrix("tet_stiffness.mat", "K");
        self.assertMatrixEqual(K_ground_truth, K);

    def test_stiffness_square(self):
        mesh = self.load_mesh("square_2D.obj");
        assembler = self.create_assembler(mesh);
        K = self.format(assembler.assemble("stiffness"));
        K_ground_truth = self.load_matrix("square_2D_stiffness.mat", "K");
        self.assertMatrixEqual(K_ground_truth, K);

    def test_mass_tet(self):
        mesh = self.load_mesh("tet.msh");
        assembler = self.create_assembler(mesh);
        M = self.format(assembler.assemble("mass"));
        M_ground_truth = self.load_matrix("tet_mass.mat", "M");
        self.assertMatrixEqual(M_ground_truth, M);

    def test_mass_square(self):
        mesh = self.load_mesh("square_2D.obj");
        assembler = self.create_assembler(mesh);
        M = self.format(assembler.assemble("mass"));
        M_ground_truth = self.load_matrix("square_2D_mass.mat", "M");
        self.assertMatrixEqual(M_ground_truth, M);

    def test_lumped_mass_tet(self):
        mesh = self.load_mesh("tet.msh");
        assembler = self.create_assembler(mesh);
        M = self.format(assembler.assemble("lumped_mass"));
        M_ground_truth = self.load_matrix("tet_lumped_mass.mat", "M");
        self.assertMatrixEqual(M_ground_truth, M);

    def test_lumped_mass_square(self):
        mesh = self.load_mesh("square_2D.obj");
        assembler = self.create_assembler(mesh);
        M = self.format(assembler.assemble("lumped_mass"));
        M_ground_truth = self.load_matrix("square_2D_lumped_mass.mat", "M");
        self.assertMatrixEqual(M_ground_truth, M);

    def test_laplacian_tet(self):
        mesh = self.load_mesh("tet.msh");
        assembler = self.create_assembler(mesh);
        L = self.format(assembler.assemble("laplacian"));
        L_ground_truth = self.load_matrix("tet_laplacian.mat", "L");
        self.assertMatrixEqual(L_ground_truth, L);

    def test_laplaican_square(self):
        mesh = self.load_mesh("square_2D.obj");
        assembler = self.create_assembler(mesh);
        L = self.format(assembler.assemble("laplacian"));
        L_ground_truth = self.load_matrix("square_2D_laplacian.mat", "L");
        self.assertMatrixEqual(L_ground_truth, L);

    def test_displacement_strain_tet(self):
        mesh = self.load_mesh("tet.msh");
        assembler = self.create_assembler(mesh);
        B = self.format(assembler.assemble("displacement_strain"));
        B_ground_truth = self.load_matrix("tet_displacement_strain.mat", "B");
        self.assertMatrixEqual(B_ground_truth, B);

    def test_displacement_strain_square(self):
        mesh = self.load_mesh("square_2D.obj");
        assembler = self.create_assembler(mesh);
        B = self.format(assembler.assemble("displacement_strain"));
        B_ground_truth = self.load_matrix("square_2D_displacement_strain.mat", "B");
        self.assertMatrixEqual(B_ground_truth, B);

    def test_elasticity_tensor_tet(self):
        mesh = self.load_mesh("tet.msh");
        assembler = self.create_assembler(mesh);
        C = self.format(assembler.assemble("elasticity_tensor"));
        C_ground_truth = self.load_matrix("tet_elasticity_tensor.mat", "C");
        self.assertMatrixEqual(C_ground_truth, C);

    def test_elasticity_tensor_square(self):
        mesh = self.load_mesh("square_2D.obj");
        assembler = self.create_assembler(mesh);
        C = self.format(assembler.assemble("elasticity_tensor"));
        C_ground_truth = self.load_matrix("square_2D_elasticity_tensor.mat", "C");
        self.assertMatrixEqual(C_ground_truth, C);

    def test_rigid_motion_tet(self):
        mesh = self.load_mesh("tet.msh");
        assembler = self.create_assembler(mesh);
        Ru = self.format(assembler.assemble("rigid_motion"));
        Ru_ground_truth = self.load_matrix("tet_rigid_motion.mat", "Ru");
        self.assertMatrixEqual(Ru_ground_truth, Ru);

    def test_rigid_motion_square(self):
        mesh = self.load_mesh("square_2D.obj");
        assembler = self.create_assembler(mesh);
        Ru = self.format(assembler.assemble("rigid_motion"));
        Ru_ground_truth = self.load_matrix("square_2D_rigid_motion.mat", "Ru");
        self.assertMatrixEqual(Ru_ground_truth, Ru);

