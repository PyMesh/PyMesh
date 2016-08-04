from pymesh.TestCase import TestCase
from pymesh import VoxelGrid
from pymesh.meshutils import generate_icosphere

class VoxelGridTest(TestCase):
    def test_sphere(self):
        mesh = generate_icosphere(2.0, [0.0, 0.0, 0.0]);
        grid = VoxelGrid(0.1);
        grid.insert_mesh(mesh);
        grid.create_grid();
        voxel_mesh = grid.mesh;

        self.assertLess(0, voxel_mesh.num_vertices);
        self.assertLess(0, voxel_mesh.num_faces);
        self.assertLess(0, voxel_mesh.num_voxels);
