void MeshTest::check_vertex_in_face_are_adjacent(const MeshPtr& mesh) {
    const size_t vertex_per_face = mesh->get_vertex_per_face();
    const size_t num_faces = mesh->get_num_faces();
    mesh->enable_vertex_connectivity();

    for (size_t fi=0; fi<num_faces; fi++) {
        const VectorI& face = mesh->get_face(fi);

        for (size_t i=0; i<vertex_per_face; i++) {
            size_t vi = face[i];
            const VectorI& neighbors_i =
                mesh->get_vertex_adjacent_vertices(vi);
            std::vector<int> neighbors_vector_i = to_vector(
                    neighbors_i.size(), neighbors_i.data());
            for (size_t j=0; j<vertex_per_face; j++) {
                if (i == j) continue;
                size_t vj = face[j];
                ASSERT_THAT(neighbors_vector_i, Contains(vj));
            }
        }
    }
}

void MeshTest::check_vertex_in_voxel_are_adjacent(const MeshPtr& mesh) {
    const size_t vertex_per_voxel = mesh->get_vertex_per_voxel();
    const size_t num_voxels = mesh->get_num_voxels();
    mesh->enable_vertex_connectivity();

    for (size_t Vi=0; Vi<num_voxels; Vi++) {
        const VectorI& voxel = mesh->get_voxel(Vi);
        for (size_t i=0; i<vertex_per_voxel; i++) {
            size_t vi = voxel[i];
            const VectorI& neighbors_i =
                mesh->get_vertex_adjacent_vertices(vi);
            std::vector<int> neighbors_i_vector = to_vector(
                    neighbors_i.size(), neighbors_i.data());
            for (size_t j=0; j<vertex_per_voxel; j++) {
                if (i == j) continue;
                size_t vj = voxel[j];
                ASSERT_THAT(neighbors_i_vector, Contains(vj));
            }
        }
    }
}

void MeshTest::check_vertex_face_adjacency(const MeshPtr& mesh) {
    const size_t vertex_per_face = mesh->get_vertex_per_face();
    const size_t num_faces = mesh->get_num_faces();
    mesh->enable_vertex_connectivity();

    for (size_t fi=0; fi<num_faces; fi++) {
        const VectorI& face = mesh->get_face(fi);

        for (size_t i=0; i<vertex_per_face; i++) {
            size_t vi = face[i];
            const VectorI& adj_faces =
                mesh->get_vertex_adjacent_faces(vi);
            std::vector<int> adj_faces_vector = to_vector(
                    adj_faces.size(), adj_faces.data());
            ASSERT_THAT(adj_faces_vector, Contains(fi));
        }
    }
}

void MeshTest::check_vertex_voxel_adjacency(const MeshPtr& mesh) {
    const size_t vertex_per_voxel = mesh->get_vertex_per_voxel();
    const size_t num_voxels = mesh->get_num_voxels();
    mesh->enable_vertex_connectivity();

    for (size_t Vi=0; Vi<num_voxels; Vi++) {
        const VectorI& voxel = mesh->get_voxel(Vi);

        for (size_t i=0; i<vertex_per_voxel; i++) {
            size_t vi = voxel[i];
            const VectorI& adj_voxels =
                mesh->get_vertex_adjacent_voxels(vi);
            std::vector<int> adj_voxels_vector = to_vector(
                    adj_voxels.size(), adj_voxels.data());
            ASSERT_THAT(adj_voxels_vector, Contains(Vi));
        }
    }
}

void MeshTest::check_vertex_vertex_adjacency_is_symmetric(const MeshPtr& mesh) {
    mesh->enable_vertex_connectivity();
    const size_t num_vertices = mesh->get_num_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        const VectorI& neighbor_vertices = mesh->get_vertex_adjacent_vertices(i);
        for (size_t j=0; j<neighbor_vertices.size(); j++) {
            const VectorI& neighbor_vertices_j = mesh->get_vertex_adjacent_vertices(
                    neighbor_vertices[j]);
            std::vector<int> neighbor_vertices_j_vector = to_vector(
                    neighbor_vertices_j.size(), neighbor_vertices_j.data());
            ASSERT_THAT(neighbor_vertices_j_vector, Contains(i));
        }
    }
}

void MeshTest::check_face_face_adjacency_is_symmetric(const MeshPtr& mesh) {
    mesh->enable_face_connectivity();
    const size_t num_faces = mesh->get_num_faces();
    for (size_t i=0; i<num_faces; i++) {
        const VectorI& neighbor_faces = mesh->get_face_adjacent_faces(i);
        for (size_t j=0; j<neighbor_faces.size(); j++) {
            size_t fj = neighbor_faces[j];
            const VectorI& neighbor_faces_j = mesh->get_face_adjacent_faces(fj);
            std::vector<int> neighbor_faces_j_vector = to_vector(
                    neighbor_faces_j.size(), neighbor_faces_j.data());
            ASSERT_THAT(neighbor_faces_j_vector, Contains(i));
        }
    }
}

void MeshTest::check_voxel_voxel_adjacency_is_symmetric(const MeshPtr& mesh) {
    mesh->enable_voxel_connectivity();
    const size_t num_voxels = mesh->get_num_voxels();
    for (size_t i=0; i<num_voxels; i++) {
        const VectorI& neighbor_voxels = mesh->get_voxel_adjacent_voxels(i);
        for (size_t j=0; j<neighbor_voxels.size(); j++) {
            size_t fj = neighbor_voxels[j];
            const VectorI& neighbor_voxels_j = mesh->get_voxel_adjacent_voxels(fj);
            std::vector<int> neighbor_voxels_j_vector = to_vector(
                    neighbor_voxels_j.size(), neighbor_voxels_j.data());
            ASSERT_THAT(neighbor_voxels_j_vector, Contains(i));
        }
    }
}

void MeshTest::check_face_voxel_adjacency(const MeshPtr& mesh) {
    mesh->enable_face_connectivity();
    const size_t num_faces = mesh->get_num_faces();
    const size_t vertex_per_face = mesh->get_vertex_per_face();
    for (size_t i=0; i<num_faces; i++) {
        const VectorI& face = mesh->get_face(i);
        const VectorI& adj_voxels = mesh->get_face_adjacent_voxels(i);
        ASSERT_LE(adj_voxels.size(), 1);
        if (adj_voxels.size() > 0) {
            size_t V_idx = adj_voxels[0];
            const VectorI& voxel = mesh->get_voxel(V_idx);
            std::vector<int> voxel_vector = to_vector(
                    voxel.size(), voxel.data());
            for (size_t k=0; k<vertex_per_face; k++) {
                ASSERT_THAT(voxel_vector, Contains(face[k]));
            }
        }
    }
}

void MeshTest::check_voxel_face_adjacency(const MeshPtr& mesh) {
    mesh->enable_voxel_connectivity();
    const size_t vertex_per_face = mesh->get_vertex_per_face();
    const size_t num_voxels = mesh->get_num_voxels();
    for (size_t i=0; i<num_voxels; i++) {
        const VectorI& voxel = mesh->get_voxel(i);
        std::vector<int> voxel_vector = to_vector(
                voxel.size(), voxel.data());
        const VectorI& adj_faces = mesh->get_voxel_adjacent_faces(i);
        for (size_t j=0; j<adj_faces.size(); j++) {
            const VectorI& face = mesh->get_face(adj_faces[j]);
            for (size_t k=0; k<vertex_per_face; k++) {
                ASSERT_THAT(voxel_vector, Contains(face[k]));
            }
        }
    }
}





