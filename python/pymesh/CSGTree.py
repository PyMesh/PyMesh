import PyMesh
import numpy as np
from .meshio import form_mesh

class CSGTree:
    """ Contructive Solid Geometry Tree.

    Perhaps the best way of describing supported operations is using context
    free grammar:

    * ``mesh`` operation: This operation is always a leaf node of the tree.

        >>> tree = pymesh.CSGTree({"mesh": mesh})

    * ``union`` operation:

        >>> tree = pymesh.CSGTree({"union":
        ...         [TREE_1, TREE_2, ..., TREE_N]
        ...     })

    * ``intersection`` operations:

        >>> tree = pymesh.CSGTree({"intersection":
        ...         [TREE_1, TREE_2, ..., TREE_N]
        ...     })

    * ``difference`` operations:

        >>> tree = pymesh.CSGTree({"difference":
        ...         [TREE_1, TREE_2]
        ...     })

    * ``symmetric_difference`` operations:

        >>> tree = pymesh.CSGTree({"symmetric_difference":
        ...         [TREE_1, TREE_2]
        ...     })

    Where ``TREE_X`` could be any of the nodes defined above.

    A tree can be build up incrementally:

        >>> left_tree = pymesh.CSGTree({"mesh": mesh_1})
        >>> right_tree = pymesh.CSGTree({"mesh": mesh_2})
        >>> tree = pymesh.CSGTree({"union": [left_tree, right_tree]})
        >>> mesh = tree.mesh

    Or constructed from a dict:

        >>> tree = pymesh.CSGTree({"union":
        ...         [{"mesh": mesh_1}, {"mesh": mesh_2}]
        ...     })
        >>> mesh = tree.mesh
    """
    def __init__(self, tree):
        """
        Args:
            tree: dictionary describing the csg tree

        """

        if isinstance(tree, CSGTree):
            self.tree = tree.tree
        elif "mesh" in tree:
            # leaf case
            mesh = tree["mesh"]
            self.tree = PyMesh.CSGTree.create_leaf("igl",
                    mesh.vertices, mesh.faces)
        elif "union" in tree:
            num_operands = len(tree["union"])
            if num_operands == 1:
                self.tree = CSGTree(tree["union"][0]).tree
            elif num_operands == 2:
                children = [ CSGTree(subtree) for subtree in tree["union"] ]
                self.tree = PyMesh.CSGTree.create("igl")
                self.tree.set_operand_1(children[0].tree)
                self.tree.set_operand_2(children[1].tree)
                self.tree.compute_union()
            elif num_operands > 2:
                mid = num_operands // 2
                child1 = CSGTree({"union": tree["union"][:mid]})
                child2 = CSGTree({"union": tree["union"][mid:]})
                self.tree = PyMesh.CSGTree.create("igl")
                self.tree.set_operand_1(child1.tree)
                self.tree.set_operand_2(child2.tree)
                self.tree.compute_union()
            else:
                raise RuntimeError("No operand provided for union operation")
        elif "intersection" in tree:
            num_operands = len(tree["intersection"])
            if num_operands == 1:
                self.tree = CSGTree(tree["intersection"][0]).tree
            elif num_operands == 2:
                children = [ CSGTree(subtree) for subtree in tree["intersection"] ]
                self.tree = PyMesh.CSGTree.create("igl")
                self.tree.set_operand_1(children[0].tree)
                self.tree.set_operand_2(children[1].tree)
                self.tree.compute_intersection()
            elif num_operands > 2:
                mid = num_operands // 2
                child1 = CSGTree({"intersection": tree["intersection"][:mid]})
                child2 = CSGTree({"intersection": tree["intersection"][mid:]})
                self.tree = PyMesh.CSGTree.create("igl")
                self.tree.set_operand_1(child1.tree)
                self.tree.set_operand_2(child2.tree)
                self.tree.compute_intersection()
            else:
                raise RuntimeError("No operand provided for intersection operation")
        elif "difference" in tree:
            children = [ CSGTree(subtree) for subtree in tree["difference"] ]
            assert(len(children) == 2)
            self.tree = PyMesh.CSGTree.create("igl")
            self.tree.set_operand_1(children[0].tree)
            self.tree.set_operand_2(children[1].tree)
            self.tree.compute_difference()
        elif "symmetric_difference" in tree:
            children = [ CSGTree(subtree) for subtree in
                    tree["symmetric_difference"] ]
            assert(len(children) == 2)
            self.tree = PyMesh.CSGTree.create("igl")
            self.tree.set_operand_1(children[0].tree)
            self.tree.set_operand_2(children[1].tree)
            self.tree.compute_symmetric_difference()
        else:
            raise NotImplementedError(
                    "Unsupported boolean operation or incorrect csg tree")

    @property
    def vertices(self):
        return self.tree.get_vertices()

    @property
    def faces(self):
        return self.tree.get_faces()

    @property
    def mesh(self):
        mesh = form_mesh(self.vertices, self.faces)
        face_sources = self.tree.get_face_sources()
        sources = self.tree.get_mesh_sources()
        mesh.add_attribute("source_face")
        mesh.set_attribute("source_face", face_sources)
        mesh.add_attribute("source")
        mesh.set_attribute("source", sources)
        return mesh

