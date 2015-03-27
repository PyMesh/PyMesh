# This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou #
import numpy as np
from numpy.linalg import norm
import os.path
import unittest

from PyMeshSetting import PYMESH_PATH
import PyMesh
import PyMeshUtils

class DuplicatedVertexRemovalTest(unittest.TestCase):
    def setUp(self):
        self.data_dir = os.path.join(PYMESH_PATH, "tests/data/");
        self.tol = 1e-3;

    def load_mesh(self, mesh_file):
        mesh_file = os.path.join(self.data_dir, mesh_file);
        if not os.path.exists(mesh_file):
            raise IOError("mesh file {} does not exist!".format(mesh_file));

        factory = PyMesh.MeshFactory();
        factory.load_file(mesh_file);
        return factory.create();

    def assert_matrix_eq(self, m1, m2):
        diff = m1 - m2;
        self.assertAlmostEqual(0.0, np.amax(diff));
        self.assertAlmostEqual(0.0, np.amin(diff));

    def check_duplicated_vertices(self, mesh):
        dim = mesh.get_dim();
        num_vertices = mesh.get_num_vertices();
        num_faces = mesh.get_num_faces();
        vertex_per_face = mesh.get_vertex_per_face();
        vertices = mesh.get_vertices().reshape((num_vertices, dim), order="C");
        faces = mesh.get_faces().reshape((num_faces, vertex_per_face), order="C");

        vertices[0,:] = vertices[1,:];

        expected_vertices = np.copy(vertices[1:,:]);
        expected_faces = np.clip(faces - 1, 0, num_faces);

        remover = PyMeshUtils.DuplicatedVertexRemoval(vertices, faces);
        remover.run(self.tol);
        result_vertices = remover.get_vertices();
        result_faces = remover.get_faces();
        index_map = remover.get_index_map();

        self.assert_matrix_eq(expected_vertices, result_vertices);
        self.assert_matrix_eq(expected_faces, result_faces);

        for i_old,i_new in enumerate(index_map):
            self.assertAlmostEqual(0.0,
                    norm(vertices[i_old] - result_vertices[i_new]),
                    self.tol);

    def test_2D(self):
        mesh = self.load_mesh("square_2D.obj");
        self.check_duplicated_vertices(mesh);

    def test_3D(self):
        mesh = self.load_mesh("cube.msh");
        self.check_duplicated_vertices(mesh);
