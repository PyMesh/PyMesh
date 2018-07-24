import PyMesh

def is_vertex_manifold(mesh):
    """ Vertex manifold check.  The result is stored as a per-vertex scalar
    field named "vertex_manifold".
    """

    vertex_manifold = PyMesh.is_vertex_manifold(mesh.faces);
    mesh.add_attribute("vertex_manifold");
    mesh.set_attribute("vertex_manifold", vertex_manifold);

def is_edge_manifold(mesh):
    """ Edge manifold check.  The result is stored as a per-edge scalar
    field named "edge_manifold".
    """

    edge_manifold = PyMesh.is_edge_manifold(mesh.faces);
    mesh.add_attribute("edge_manifold");
    mesh.set_attribute("edge_manifold", edge_manifold);
