import PyWindingNumber

def compute_winding_number(mesh, queries, engine="auto"):
    assert(mesh.dim == 3);
    assert(mesh.vertex_per_face == 3);

    if engine == "auto":
        engine = "igl";

    engine = PyWindingNumber.WindingNumberEngine.create(engine);
    engine.set_mesh(mesh.vertices, mesh.faces);
    winding_numbers = engine.run(queries).ravel();

    return winding_numbers;
