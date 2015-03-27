#!/usr/bin/env python
# This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou #

import numpy as np
import os.path
import unittest

from PyMeshSetting import PYMESH_PATH
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

    def test_bd_nodes_raw_sqaure_raw_data(self):
        mesh = self.load_mesh("square_2D.obj");
        vertices = mesh.get_vertices().reshape((-1, 2), order="C");
        faces = mesh.get_faces().reshape((-1, 3), order="C");
        boundary = PyMeshUtils.Boundary.extract_surface_boundary_raw(vertices, faces);
        bd_nodes = boundary.get_boundary_nodes();
        bd_nodes = bd_nodes.ravel();
        self.assertEqual(4, len(bd_nodes));
        self.assertSetEqual(set(range(4)), set(bd_nodes));

    def test_bd_nodes_tet_raw_data(self):
        mesh = self.load_mesh("tet.msh");
        vertices = mesh.get_vertices().reshape((-1, 3), order="C");
        voxels = mesh.get_voxels().reshape((-1, 4), order="C")
        boundary = PyMeshUtils.Boundary.extract_volume_boundary_raw(
                vertices, voxels);
        bd_nodes = boundary.get_boundary_nodes();
        bd_nodes = bd_nodes.ravel();
        self.assertEqual(4, len(bd_nodes));
        self.assertSetEqual(set(range(4)), set(bd_nodes));

