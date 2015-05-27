from pymesh.meshutils import generate_box_mesh
from pymesh.TestCase import TestCase

import numpy as np

class GenerateBoxMeshTest(TestCase):
    def assert_bbox_matches(self, mesh, bbox_min, bbox_max):
        bbox = mesh.bbox;
        self.assert_array_equal(bbox_min, bbox[0]);
        self.assert_array_equal(bbox_max, bbox[1]);

    def test_3D(self):
        bbox_min = np.zeros(3);
        bbox_max = np.ones(3);
        mesh = generate_box_mesh(
                bbox_min, bbox_max);

        self.assertEqual(8, mesh.num_vertices);
        self.assertEqual(12, mesh.num_faces);
        self.assertEqual(6, mesh.num_voxels);

        self.assert_bbox_matches(mesh, bbox_min, bbox_max);

    def test_2D(self):
        bbox_min = np.zeros(2);
        bbox_max = np.ones(2) * 100.5;

        mesh = generate_box_mesh(
                bbox_min, bbox_max);

        self.assertEqual(4, mesh.num_vertices);
        self.assertEqual(2, mesh.num_faces);

        self.assert_bbox_matches(mesh, bbox_min, bbox_max);

    def test_samples(self):
        bbox_min = np.zeros(3);
        bbox_max = np.ones(3);
        mesh = generate_box_mesh(
                bbox_min, bbox_max, num_samples=2);

        self.assertEqual(27, mesh.num_vertices);
        self.assertEqual(48, mesh.num_faces);
        self.assertEqual(48, mesh.num_voxels);

        # There is a total of 8 cells.
        cell_index = mesh.get_attribute("cell_index");
        self.assertEqual(0, np.amin(cell_index));
        self.assertEqual(7, np.amax(cell_index));

        self.assert_bbox_matches(mesh, bbox_min, bbox_max);

    def test_symmetric_connectivity(self):
        bbox_min = np.zeros(3);
        bbox_max = np.ones(3);
        mesh = generate_box_mesh(
                bbox_min, bbox_max, keep_symmetry=True);

        self.assertEqual(15, mesh.num_vertices);
        self.assertEqual(24, mesh.num_faces);
        self.assertEqual(24, mesh.num_voxels);

        self.assert_bbox_matches(mesh, bbox_min, bbox_max);

    def test_subdiv(self):
        bbox_min = np.zeros(3);
        bbox_max = np.ones(3);
        mesh = generate_box_mesh(
                bbox_min, bbox_max, subdiv_order=1);

        self.assertEqual(27, mesh.num_vertices);
        self.assertEqual(48, mesh.num_faces);
        self.assertEqual(48, mesh.num_voxels);

        # All tets belongs to the same cell.
        cell_index = mesh.get_attribute("cell_index");
        self.assertEqual(0, np.amax(cell_index));
        self.assertEqual(0, np.amin(cell_index));

        self.assert_bbox_matches(mesh, bbox_min, bbox_max);

