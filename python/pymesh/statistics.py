import PyMesh
from time import time

__all__ = ["statistics"]


def statistics(mesh, engine="auto", with_timing=False):
    """ Computes statistics of the input meshes.

    Args:
        mesh (:class:`Mesh`): The input mesh, :math:`M_1`.

        engine (``string``): (optional) Statistics engine name.  Valid engines include:

            * ``auto``: Using the default boolean engine
              (``cgal`` by default).  This is the default.
            * ``cgal``: `CGAL 3D boolean operations on Nef Polyhedra
              <http://doc.cgal.org/latest/Nef_3/index.html>`_

        with_timing (``boolean``): (optional) Whether to time the code.

    Returns: Mesh Statistics PyMesh.Statistics
    """
    assert (mesh.vertex_per_face == 3)

    if engine == "auto":
        engine = "cgal"

    engine = PyMesh.StatisticsEngine.create(engine)
    engine.set_mesh(mesh.vertices, mesh.faces)

    if with_timing:
        start_time = time()

    mesh_statistics: PyMesh.MeshStatistics = engine.compute_statistics()

    if with_timing:
        finish_time = time()
        running_time = finish_time - start_time

    if with_timing:
        return mesh_statistics, running_time
    else:
        return mesh_statistics
