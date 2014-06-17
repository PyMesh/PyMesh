#!/usr/bin/env python

import numpy as np
import os.path
import unittest

from PyMeshSetting import PYMESH_PATH
import PyMesh
import PyMeshUtils

class SubdivisionTest(unittest.TestCase):
    def setUp(self):
        self.data_dir = os.path.join(PYMESH_PATH, "tests/data/");

    def load_mesh(self, mesh_file):
        mesh_file = os.path.join(self.data_dir, mesh_file);
        if not os.path.exists(mesh_file):
            raise IOError("mesh file {} does not exist!".format(mesh_file));

        factory = PyMesh.MeshFactory();
        factory.load_file(mesh_file);
        factory.with_attribute("face_area");
        return factory.create();

    def form_mesh(self, vertices, faces):
        factory = PyMesh.MeshFactory();
        factory.load_data(
                vertices.ravel(order="C"),
                faces.ravel(order="C"),
                np.array([]),
                vertices.shape[1], faces.shape[1], 4);
        factory.with_attribute("face_area");
        return factory.create();

    def test_simple_subdivision(self):
        mesh = self.load_mesh("tet.msh");
        vertices = mesh.get_vertices().reshape((-1,3));
        faces = mesh.get_faces().reshape((-1, 3));
        self.assertEqual(4, vertices.shape[0]);
        self.assertEqual(4, faces.shape[0]);

        sub = PyMeshUtils.Subdivision.create("simple");
        sub.subdivide(vertices, faces, 1);

        sub_vertices = sub.get_vertices();
        sub_faces = sub.get_faces();

        self.assertEqual(10, sub_vertices.shape[0]);
        self.assertEqual(16, sub_faces.shape[0]);

    def test_const_total_area(self):
        mesh = self.load_mesh("cube.obj");
        vertices = mesh.get_vertices().reshape((-1,3));
        faces = mesh.get_faces().reshape((-1, 3));
        area = np.sum(mesh.get_attribute("face_area"));

        sub = PyMeshUtils.Subdivision.create("simple");
        sub.subdivide(vertices, faces, 1);

        sub_mesh = self.form_mesh(sub.get_vertices(), sub.get_faces());
        sub_area = np.sum(sub_mesh.get_attribute("face_area"));

        self.assertAlmostEqual(area, sub_area);



