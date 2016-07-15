import PyEnvelope
from .Mesh import Mesh

class VoxelGrid:
    def __init__(self, cell_size, dim=3):
        self.dim = dim;
        if dim == 3:
            self.raw_grid = PyEnvelope.VoxelGrid3D(cell_size);
        elif dim == 2:
            self.raw_grid = PyEnvelope.VoxelGrid2D(cell_size);
        else:
            raise NotImplementedError("Unsupported dim: {}".format(dim));

    def insert_mesh(self, mesh):
        assert(self.dim == mesh.dim);
        self.raw_grid.insert_mesh(mesh.raw_mesh);

    def erode(self, itrs):
        self.raw_grid.erode(itrs);

    def dilate(self, itrs):
        self.raw_grid.dilate(itrs);

    @property
    def mesh(self):
        self.raw_grid.create_grid();
        return Mesh(self.raw_grid.get_voxel_mesh());
