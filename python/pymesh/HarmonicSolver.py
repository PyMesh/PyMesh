import PyMesh
from .meshio import form_mesh

class HarmonicSolver(PyMesh.HarmonicSolver):
    @classmethod
    def create(cls, mesh):
        """ Create a harmonic solver using the input mesh as domain.
        """
        return PyMesh.HarmonicSolver.create(mesh.raw_mesh);

    @property
    def mesh(self):
        nodes = self.nodes;
        elements = self.elements;
        if nodes.shape[1] == 2:
            assert(elements.shape[1] == 3);
            return form_mesh(nodes, elements);
        else:
            assert(nodes.shape[1] == 3);
            assert(elements.shape[1] == 4);
            return form_mesh(nodes, np.array([]), elements);

    @mesh.setter
    def mesh(self, mesh):
        dim = mesh.dim;
        if dim == 2:
            self.nodes = mesh.vertices;
            self.elements = mesh.faces;
        else:
            assert(dim == 3);
            assert(mesh.num_voxels > 0);
            self.nodes = mesh.vertices;
            self.elements = mesh.voxels;

