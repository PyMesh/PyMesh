import PyBoolean
import numpy as np
from .meshio import form_mesh

class CSGTree:
    def __init__(self, tree):
        """
        Args:
            tree: dictionary describing the csg tree

        Example:

        A simple binary union:
            
            {
                "union": [ { "mesh": mesh_1}, { "mesh": mesh_2} ]
            }


        Difference of two unions:

            {
                "difference" [
                {
                    "union": [ { "mesh": mesh_1}, { "mesh": mesh_2} ]
                },
                {
                    "union": [ { "mesh": mesh_3}, { "mesh": mesh_4} ]
                }
                ]
            }
        """

        if "mesh" in tree:
            # leaf case
            mesh = tree["mesh"];
            self.tree = PyBoolean.CSGTree.create_leaf("igl",
                    mesh.vertices, mesh.faces);
        elif "union" in tree:
            children = [ CSGTree(subtree) for subtree in tree["union"] ];
            assert(len(children) == 2);
            self.tree = PyBoolean.CSGTree.create("igl");
            self.tree.set_operand_1(children[0].tree);
            self.tree.set_operand_2(children[1].tree);
            self.tree.compute_union();
        elif "intersection" in tree:
            children = [ CSGTree(subtree) for subtree in tree["intersection"] ];
            assert(len(children) == 2);
            self.tree = PyBoolean.CSGTree.create("igl");
            self.tree.set_operand_1(children[0].tree);
            self.tree.set_operand_2(children[1].tree);
            self.tree.compute_intersection();
        elif "difference" in tree:
            children = [ CSGTree(subtree) for subtree in tree["difference"] ];
            assert(len(children) == 2);
            self.tree = PyBoolean.CSGTree.create("igl");
            self.tree.set_operand_1(children[0].tree);
            self.tree.set_operand_2(children[1].tree);
            self.tree.compute_difference();
        elif "symmetric_difference" in tree:
            children = [ CSGTree(subtree) for subtree in
                    tree["symmetric_difference"] ];
            assert(len(children) == 2);
            self.tree = PyBoolean.CSGTree.create("igl");
            self.tree.set_operand_1(children[0].tree);
            self.tree.set_operand_2(children[1].tree);
            self.tree.compute_symmetric_difference();
        else:
            raise NotImplementedError(
                    "Unsupported boolean operation or incorrect csg tree");

    @property
    def vertices(self):
        return self.tree.get_vertices();

    @property
    def faces(self):
        return self.tree.get_faces();

    @property
    def mesh(self):
        return form_mesh(self.vertices, self.faces);

