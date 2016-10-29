import PyTetrahedronization
from .meshio import form_mesh

def tetrahedralize(mesh, face_size, cell_size, engine="auto"):
    if mesh.dim != 3:
        raise NotImplementedError("Tetrahedralization only works with 3D mesh");
    if mesh.vertex_per_face != 3:
        raise NotImplementedError("Only triangle mesh is supported for now");

    vertices = mesh.vertices.reshape((-1, 3), order="C");
    faces = mesh.faces.reshape((-1, 3), order="C").astype(int);

    engine = PyTetrahedronization.TetrahedronizationEngine.create(engine);
    engine.set_vertices(vertices);
    engine.set_faces(faces);
    engine.set_face_size(face_size);
    engine.set_cell_size(cell_size);
    engine.run();

    vertices = engine.get_vertices();
    faces = engine.get_faces();
    voxels = engine.get_voxels();

    return form_mesh(vertices, faces, voxels);
