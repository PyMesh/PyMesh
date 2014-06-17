#!/usr/bin/env python

import numpy as np
from numpy.linalg import norm
import os.path
import unittest

from PyMeshSetting import PYMESH_PATH
import PyMesh
import PyMeshUtils

class PointLocatorTest(unittest.TestCase):
    def setUp(self):
        self.data_dir = os.path.join(PYMESH_PATH, "tests/data/");

    def load_mesh(self, mesh_file):
        mesh_file = os.path.join(self.data_dir, mesh_file);
        if not os.path.exists(mesh_file):
            raise IOError("mesh file {} does not exist!".format(mesh_file));

        factory = PyMesh.MeshFactory();
        factory.load_file(mesh_file);
        return factory.create();

    def assert_barycentric_coords_are_valid(self, mesh, elem_indices,
            barycentric_coords, targets):
        dim = mesh.get_dim();
        vertices = mesh.get_vertices().reshape((-1, dim), order="C");
        if dim == 2:
            elements = mesh.get_faces().reshape(
                    (-1, mesh.get_vertex_per_face()), order="C");
        elif dim == 3:
            elements = mesh.get_voxels().reshape(
                    (-1, mesh.get_vertex_per_voxel()), order="C");
        else:
            raise NotImplementedError(
                    "{}D mesh is not supported".format(dim));

        self.assertEqual(elements.shape[1], barycentric_coords.shape[1]);
        elements = elements[elem_indices];
        points = np.zeros_like(targets);
        for i in range(dim+1):
            points += vertices[elements[:,i]] * barycentric_coords[:,i][:,np.newaxis]

        error = norm(points - targets);
        self.assertAlmostEqual(0.0, error);

    def test_2D(self):
        mesh = self.load_mesh("square_2D.obj");
        x = np.linspace(-1.0, 1.0, 10);
        y = np.linspace(-1.0, 1.0, 10);
        x_coord, y_coord = np.meshgrid(x, y);
        points = np.array([x_coord.ravel(), y_coord.ravel()]).T;

        p_locator = PyMeshUtils.PointLocator(mesh);
        p_locator.locate(points);

        elem_indices = p_locator.get_enclosing_voxels().ravel();
        barycentric_coords = p_locator.get_barycentric_coords();

        self.assert_barycentric_coords_are_valid(mesh, elem_indices,
                barycentric_coords, points);

    def test_3D(self):
        mesh = self.load_mesh("cube.msh");
        x = np.linspace(-1.0, 1.0, 10);
        y = np.linspace(-1.0, 1.0, 10);
        z = np.linspace(-1.0, 1.0, 10);
        x_coord, y_coord, z_coord = np.meshgrid(x, y, z);
        points = np.array([x_coord.ravel(), y_coord.ravel(), z_coord.ravel()]).T;

        p_locator = PyMeshUtils.PointLocator(mesh);
        p_locator.locate(points);

        elem_indices = p_locator.get_enclosing_voxels().ravel();
        barycentric_coords = p_locator.get_barycentric_coords();

        self.assert_barycentric_coords_are_valid(mesh, elem_indices,
                barycentric_coords, points);

    def test_outside(self):
        mesh = self.load_mesh("tet.msh");
        x = np.linspace(-1.0, 1.0, 10);
        y = np.linspace(-1.0, 1.0, 10);
        z = np.linspace(-1.0, 1.0, 10);
        x_coord, y_coord, z_coord = np.meshgrid(x, y, z);
        points = np.array([x_coord.ravel(), y_coord.ravel(), z_coord.ravel()]).T;

        p_locator = PyMeshUtils.PointLocator(mesh);
        self.assertRaises(RuntimeError, p_locator.locate, points);

    def test_zero_elements(self):
        mesh = self.load_mesh("tet.obj");
        self.assertRaises(RuntimeError, PyMeshUtils.PointLocator, mesh);

