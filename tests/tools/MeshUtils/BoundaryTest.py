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

class BoundaryTest(unittest.TestCase):
    def setUp(self):
        self.data_dir = os.path.join(PYMESH_PATH, "tests/data/");

    def load_mesh(self, mesh_file):
        mesh_file = os.path.join(self.data_dir, mesh_file);
        if not os.path.exists(mesh_file):
            raise IOError("mesh file {} does not exist!".format(mesh_file));

        factory = PyMesh.MeshFactory();
        factory.load_file(mesh_file);
        return factory.create();

    def test_bd_nodes_tet(self):
        mesh = self.load_mesh("tet.msh");
        boundary = PyMeshUtils.Boundary.extract_volume_boundary(mesh);
        bd_nodes = boundary.get_boundary_nodes();
        bd_nodes = bd_nodes.ravel();
        self.assertEqual(4, len(bd_nodes));
        self.assertSetEqual(set(range(4)), set(bd_nodes));

    def test_bd_elements_tet(self):
        mesh = self.load_mesh("tet.msh");
        boundary = PyMeshUtils.Boundary.extract_volume_boundary(mesh);
        bd_faces = boundary.get_boundaries();
        self.assertTupleEqual((4,3), bd_faces.shape);

        for i in range(bd_faces.shape[0]):
            voxel_idx = boundary.get_boundary_element(i);
            voxel = mesh.get_voxel(voxel_idx);
            face = boundary.get_boundary(i);
            voxel = set(voxel.ravel());
            face = set(face.ravel());
            self.assertSetEqual(voxel & face, face);

    def test_bd_nodes_square(self):
        mesh = self.load_mesh("square_2D.obj");
        boundary = PyMeshUtils.Boundary.extract_surface_boundary(mesh);
        bd_nodes = boundary.get_boundary_nodes();
        bd_nodes = bd_nodes.ravel();
        self.assertEqual(4, len(bd_nodes));
        self.assertSetEqual(set(range(4)), set(bd_nodes));

    def test_bd_elements_square(self):
        mesh = self.load_mesh("square_2D.obj");
        boundary = PyMeshUtils.Boundary.extract_surface_boundary(mesh);
        bd_faces = boundary.get_boundaries();
        self.assertTupleEqual((4,2), bd_faces.shape);

        for i in range(bd_faces.shape[0]):
            face_idx = boundary.get_boundary_element(i);
            face = mesh.get_face(face_idx);
            edge = boundary.get_boundary(i);
            face = set(face.ravel());
            edge = set(edge.ravel());
            self.assertSetEqual(face & edge, edge);

