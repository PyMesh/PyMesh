import PyMesh
import numpy as np

from .Mesh import Mesh

def compress(mesh, engine_name="draco"):
    engine = PyMesh.CompressionEngine.create(engine_name);
    data = engine.compress(mesh.raw_mesh);
    return data;

def decompress(data, engine_name="draco"):
    engine = PyMesh.CompressionEngine.create(engine_name);
    return Mesh(engine.decompress(data));
