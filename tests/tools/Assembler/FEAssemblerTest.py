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

    def test_stiffness_tet(self):
        mesh = self.load_mesh("tet.msh");
        assembler = PyAssembler.FEAssembler(mesh, self.material_name);
        K = self.format(assembler.assemble("stiffness"));
        K_ground_truth = self.load_matrix("tet_stiffness.mat", "K");
        self.assertMatrixEqual(K_ground_truth, K);

    def test_stiffness_square(self):
        mesh = self.load_mesh("square_2D.obj");
        assembler = PyAssembler.FEAssembler(mesh, self.material_name);
        K = self.format(assembler.assemble("stiffness"));
        K_ground_truth = self.load_matrix("square_2D_stiffness.mat", "K");
        self.assertMatrixEqual(K_ground_truth, K);





