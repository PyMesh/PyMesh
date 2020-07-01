import PyMesh
import numpy as np

class AABBTree:
    def __init__(self):
        self.__raw_tree = None

    def load_data(self, vertices, faces):
        self.__raw_tree = PyMesh.AABBTree(vertices, faces)

    def load_mesh(self, mesh):
        self.__raw_tree = PyMesh.AABBTree(mesh.vertices, mesh.faces)

    def look_up(self, pts):
        sq_dists, face_indices = \
                self.__raw_tree.look_up(pts)
        return sq_dists.squeeze(), face_indices.squeeze()

    def look_up_with_closest_points(self, pts):
        sq_dists, face_indices, closest_pts = \
                self.__raw_tree.look_up_with_closest_points(pts)
        return sq_dists.squeeze(), face_indices.squeeze(), closest_pts

    def do_intersect_segments(self, vertices, edges):
        return self.__raw_tree.do_intersect_segments(vertices, edges).squeeze()

class AABBTree2:
    def __init__(self):
        self.__raw_tree = None

    def load_data(self, points, segments):
        self.__raw_tree = PyMesh.AABBTree2(points, segments)

    def look_up(self, pts):
        sq_dists, segment_indices = \
                self.__raw_tree.look_up(pts)
        return sq_dists.squeeze(), segment_indices.squeeze()

    def look_up_with_closest_points(self, pts):
        sq_dists, segment_indices, closest_pts = \
                self.__raw_tree.look_up_with_closest_points(pts)
        return sq_dists.squeeze(), segment_indices.squeeze(), closest_pts

class BVH:
    available_engines = PyMesh.BVHEngine.available_engines

    def __init__(self, engine_name="auto", dim=3):
        self.__raw_bvh = PyMesh.BVHEngine.create(engine_name, dim)

    def load_data(self, vertices, faces):
        self.__raw_bvh.set_mesh(vertices, faces)
        self.__raw_bvh.build()

    def load_mesh(self, mesh):
        self.__raw_bvh.set_mesh(mesh.vertices, mesh.faces)
        self.__raw_bvh.build()

    def lookup(self, pts):
        sq_dists, face_indices, closest_pts = self.__raw_bvh.lookup(pts)
        return sq_dists.squeeze(), face_indices.squeeze(), closest_pts

    def lookup_signed(self, pts, fn, vn, en, emap):
        signed_dists, face_indices, closest_pts, face_normals = self.__raw_bvh.lookup_signed(pts, fn, vn, en, emap)
        return signed_dists, face_indices.squeeze(), closest_pts, face_normals.squeeze()


def distance_to_mesh(mesh, pts, engine="auto", bvh=None):
    """ Compute the distance from a set of points to a mesh.

    Args:
        mesh (:class:`Mesh`): A input mesh.
        pts (:class:`numpy.ndarray`): A :math:`N \\times dim` array of query
            points.
        engine (``string``): BVH engine name. Valid choices are "cgal",
            "geogram", "igl" if all dependencies are used. The default is
            "auto" where an available engine is automatically picked.
        bvh (:class:`BVH`): BVH engine instance (optional)

    Returns:
        Three values are returned.

            * ``squared_distances``: squared distances from each point to mesh.
            * ``face_indices``  : the closest face to each point.
            * ``closest_points``: the point on mesh that is closest to each
                                  query point.
    """

    if not bvh:
        bvh = BVH(engine, mesh.dim)
        bvh.load_mesh(mesh)
    squared_distances, face_indices, closest_points = bvh.lookup(pts)
    return squared_distances, face_indices, closest_points

def signed_distance_to_mesh(mesh, pts, engine="igl", bvh=None):
    """ Compute the signed distance from a set of points to a mesh.

    Args:
        mesh (:class:`Mesh`): A input mesh.
        pts (:class:`numpy.ndarray`): A :math:`N \\times dim` array of query
            points.
        engine (``string``): BVH engine name. Valid choices are "cgal",
            "geogram", "igl" if all dependencies are used. The default is
            "auto" where an available engine is automatically picked.
        bvh (:class:`BVH`): BVH engine instance (optional)

    Returns:
        Three values are returned.

            * ``signed_distances``: signed (unsquared) distances from each
                                    point to mesh.
            * ``face_indices``  : the closest face to each point.
            * ``closest_points``: the point on mesh that is closest to each
                                  query point.
    """

    if not bvh:
        bvh = BVH(engine, mesh.dim)
        bvh.load_mesh(mesh)

    # get face normals
    try:
        face_normals = np.reshape(mesh.get_attribute("face_normals"),
                                  np.int32(mesh.get_attribute("face_normals_shape")))
    except RuntimeError:
        face_normals = PyMesh.face_normals(mesh.vertices, mesh.faces)
        mesh.add_attribute("face_normals")
        mesh.add_attribute("face_normals_shape")
        mesh.set_attribute("face_normals", face_normals)
        mesh.set_attribute("face_normals_shape", np.array(face_normals.shape))

    # get vertex normals
    try:
        vertex_normals = np.reshape(mesh.get_attribute("vertex_normals"),
                                    np.int32(mesh.get_attribute("vertex_normals_shape")))
    except RuntimeError:
        vertex_normals = PyMesh.vertex_normals(mesh.vertices, mesh.faces, face_normals)
        mesh.add_attribute("vertex_normals")
        mesh.add_attribute("vertex_normals_shape")
        mesh.set_attribute("vertex_normals", vertex_normals)
        mesh.set_attribute("vertex_normals_shape", np.array(vertex_normals.shape))

    # get edge normals
    try:
        edge_normals = np.reshape(mesh.get_attribute("edge_normals"),
                                  np.int32(mesh.get_attribute("edge_normals_shape")))
        edge_map = np.reshape(np.int32(mesh.get_attribute("edge_map")),
                              np.int32(mesh.get_attribute("edge_map_shape")))
    except RuntimeError:
        edge_normals, _, edge_map = PyMesh.edge_normals(mesh.vertices, mesh.faces, face_normals)
        mesh.add_attribute("edge_normals")
        mesh.add_attribute("edge_map")
        mesh.add_attribute("edge_normals_shape")
        mesh.add_attribute("edge_map_shape")
        mesh.set_attribute("edge_normals", edge_normals)
        mesh.set_attribute("edge_map", edge_map)
        mesh.set_attribute("edge_normals_shape", np.array(edge_normals.shape))
        mesh.set_attribute("edge_map_shape", np.array(edge_map.shape))

    signed_distances, face_indices, closest_points, face_normals = bvh.lookup_signed(pts, face_normals, vertex_normals, edge_normals, edge_map)
    return signed_distances, face_indices, closest_points, face_normals

def do_intersect(mesh, nodes, elements):
    """ Check if each element intersects the mesh.

    Args:
        mesh (:class:`Mesh`): Input mesh.
        nodes(:class:`numpy.ndarray`): The nodes of the elements.
        elements (:class:`numpy.ndarray`): Connectivity of the nodes.

    Returns:
        A list indicating if each element intersects the mesh.
    """
    tree = AABBTree()
    tree.load_mesh(mesh)

    assert(elements.ndim == 2)
    elem_type = elements.shape[1]
    if (elem_type == 2):
        r =tree.do_intersect_segments(nodes, elements) != 0
    else:
        raise NotImplementedError(
                "AABB tree does not support element consisting of {} nodes"\
                        .format(elem_type))
    return r


