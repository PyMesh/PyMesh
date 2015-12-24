from pymesh.TestCase import TestCase
from pymesh import generate_box_mesh
from pymesh import hex_to_tet
from pymesh import form_mesh
import numpy as np

class MeshIOTest(TestCase):
    def write_and_load(self, mesh, filename,
            attr_names=[], use_ascii=False, anonymous=False):
        setting = {};
        setting["ascii"] = use_ascii;
        setting["anonymous"] = anonymous;

        filename = self.save_mesh(filename, mesh, *attr_names, **setting);
        mesh2 = self.load_mesh(filename);
        return mesh2;

    def get_box_mesh(self):
        mesh = generate_box_mesh(np.zeros(3), np.ones(3));
        mesh = form_mesh(mesh.vertices, mesh.faces);
        return mesh;

    def test_obj(self):
        mesh = self.get_box_mesh();
        mesh2 = self.write_and_load(mesh, "cube.obj");
        self.assert_mesh_equal(mesh, mesh2);

    def test_ply(self):
        mesh = self.get_box_mesh();
        mesh.add_attribute("vertex_index");

        mesh2 = self.write_and_load(mesh, "cube.ply",
                attr_names=["vertex_index"]);
        self.assert_mesh_equal(mesh, mesh2, attr_names=["vertex_index"]);

        mesh3 = self.write_and_load(mesh, "cube.ply",
                attr_names=["vertex_index"], use_ascii=True);
        self.assert_mesh_equal(mesh, mesh3, attr_names=["vertex_index"]);

    def test_msh(self):
        mesh = generate_box_mesh(np.zeros(3), np.ones(3));
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

    def test_anonymous(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            ]);
        faces = np.array([[0, 1, 2]]);
        mesh = form_mesh(vertices, faces);

        for ext in [".msh", ".obj", ".ply", ".mesh", ".off"]:
            mesh1 = self.write_and_load(mesh,
                    "anonymous_test{}".format(ext), use_ascii=True,
                    anonymous=True);
            self.assert_mesh_equal(mesh, mesh1);
            mesh2 = self.write_and_load(mesh,
                    "anonymous_test{}".format(ext), use_ascii=False,
                    anonymous=True);
            self.assert_mesh_equal(mesh, mesh2);

