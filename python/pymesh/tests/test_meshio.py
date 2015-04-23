from pymesh.TestCase import TestCase

class MeshIOTest(TestCase):
    def write_and_load(self, mesh, filename,
            attr_names=[], use_ascii=False):
        if use_ascii:
            setting = {"ascii": True};
        else:
            setting = {};

        filename = self.save_mesh(filename, mesh, *attr_names, **setting);
        mesh2 = self.load_mesh(filename);
        return mesh2;

    def test_obj(self):
        mesh = self.load_mesh("cube.obj");
        self.assertEqual(3, mesh.dim);
        self.assertEqual(3, mesh.vertex_per_face);
        self.assertEqual(8, mesh.num_vertices);
        self.assertEqual(12, mesh.num_faces);
        self.assertEqual(0, mesh.num_voxels);

        mesh2 = self.write_and_load(mesh, "cube.obj");
        self.assert_mesh_equal(mesh, mesh2);

    def test_ply(self):
        mesh = self.load_mesh("cube.ply");
        self.assertEqual(3, mesh.dim);
        self.assertEqual(3, mesh.vertex_per_face);
        self.assertEqual(8, mesh.num_vertices);
        self.assertEqual(12, mesh.num_faces);
        self.assertEqual(0, mesh.num_voxels);
        mesh.add_attribute("vertex_index");

        mesh2 = self.write_and_load(mesh, "cube.ply",
                attr_names=["vertex_index"]);
        self.assert_mesh_equal(mesh, mesh2, attr_names=["vertex_index"]);

        mesh3 = self.write_and_load(mesh, "cube.ply",
                attr_names=["vertex_index"], use_ascii=True);
        self.assert_mesh_equal(mesh, mesh3, attr_names=["vertex_index"]);

    def test_msh(self):
        mesh = self.load_mesh("cube.msh");
        self.assertEqual(3, mesh.dim);
        self.assertEqual(3, mesh.vertex_per_face);
        self.assertEqual(8, mesh.num_vertices);
        self.assertEqual(12, mesh.num_faces);
        self.assertEqual(6, mesh.num_voxels);
        mesh.add_attribute("vertex_index");

        mesh2 = self.write_and_load(mesh, "cube.msh",
                attr_names=["vertex_index"]);
        self.assert_mesh_equal(mesh, mesh2, attr_names=["vertex_index"]);

        mesh3 = self.write_and_load(mesh, "cube.msh",
                attr_names=["vertex_index"], use_ascii=True);
        self.assert_mesh_equal(mesh, mesh3, attr_names=["vertex_index"]);

