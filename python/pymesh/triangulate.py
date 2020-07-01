import PyMesh
from .meshio import form_mesh
import numpy as np
from time import time

def triangulate_beta(points, segments, engine="auto", with_timing=False):
    if engine == "auto":
        engine = "shewchuk_triangle"

    engine = PyMesh.Triangulation.create(engine)
    engine.set_points(points)
    engine.set_segments(segments)

    if with_timing:
        start_time = time()

    engine.run()

    if with_timing:
        finish_time = time()
        running_time = finish_time - start_time

    vertices = engine.get_vertices()
    faces = engine.get_faces()

    mesh = form_mesh(vertices, faces)
    if with_timing:
        return mesh, running_time
    else:
        return mesh


def refine_triangulation(mesh, metrics=None, engine="auto", with_timing=False):
    if engine == "auto":
        engine = "mmg_delaunay"

    engine = PyMesh.Triangulation.create(engine)
    engine.set_vertices(mesh.vertices)
    engine.set_faces(mesh.faces)

    if with_timing:
        start_time = time()

    if metrics is None:
        engine.refine(np.zeros((0,1)))
    else:
        engine.refine(metrics)

    if with_timing:
        finish_time = time()
        running_time = finish_time - start_time

    vertices = engine.get_vertices()
    faces = engine.get_faces()

    mesh = form_mesh(vertices, faces)
    if with_timing:
        return mesh, running_time
    else:
        return mesh
