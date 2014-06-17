import numpy as np
import os.path
import unittest

from PyMeshSetting import PYMESH_PATH
import PyMesh
import PyMeshUtils

class IsolatedVertexRemovalTest(unittest.TestCase):
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

    def add_extra_vertex_and_check(self, mesh):
        dim = mesh.get_dim();
        num_vertices = mesh.get_num_vertices();
        num_faces = mesh.get_num_faces();
        vertex_per_face = mesh.get_vertex_per_face();
        vertices = mesh.get_vertices().reshape((num_vertices, dim), order="C");
        faces = mesh.get_faces().reshape((num_faces, vertex_per_face), order="C");

        modified_vertices = np.vstack((np.ones(dim), vertices));
        modified_faces = faces + 1;

        remover = PyMeshUtils.IsolatedVertexRemoval(modified_vertices, modified_faces);
        remover.run();
        result_vertices = remover.get_vertices();
        result_faces = remover.get_faces();

        self.assert_matrix_eq(vertices, result_vertices);
        self.assert_matrix_eq(faces, result_faces);

    def test_2D(self):
        mesh = self.load_mesh("square_2D.obj");
        self.add_extra_vertex_and_check(mesh);

    def test_3D(self):
        mesh = self.load_mesh("cube.msh");
        self.add_extra_vertex_and_check(mesh);
