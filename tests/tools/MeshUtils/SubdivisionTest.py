#!/usr/bin/env python

import numpy as np
import os
import os.path
import sys
import unittest

PYMESH_PATH = os.environ.get("PYMESH_PATH");
if PYMESH_PATH is None:
    raise ImportError("Please set PYMESH_PATH to the correct lib path.")
sys.path.append(os.path.join(PYMESH_PATH, "lib"));
sys.path.append(os.path.join(PYMESH_PATH, "swig"));

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
