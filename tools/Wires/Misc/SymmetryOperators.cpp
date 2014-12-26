#include "SymmetryOperators.h"

template<>
SymmetryOperators::Operators SymmetryOperators::generate_reflective_symmetry_operators<2>(const VectorF& c) {
    assert(c.size() == 2);
    Operators ops;

    // Reflective symmetry with respect to X and Y axis assuming origin at c.
    ops.push_back([=](const VectorF& p) { return Vector2F(2*c[0]-p[0],        p[1]); });
    ops.push_back([=](const VectorF& p) { return Vector2F(       p[0], 2*c[1]-p[1]); });

    // Reflective symmetry with respect to center.
    ops.push_back([=](const VectorF& p) { return Vector2F(2*c[0]-p[0], 2*c[1]-p[1]); });

    return ops;
}

template<>
SymmetryOperators::Operators SymmetryOperators::generate_reflective_symmetry_operators<3>(const VectorF& c) {
    assert(c.size() == 3);
    Operators ops;

    // Reflective symmetry with respect to YZ, ZX and XY plane.
    ops.push_back([=](const VectorF& p) { return Vector3F(2*c[0]-p[0],        p[1],        p[2]); });
    ops.push_back([=](const VectorF& p) { return Vector3F(       p[0], 2*c[1]-p[1],        p[2]); });
    ops.push_back([=](const VectorF& p) { return Vector3F(       p[0],        p[1], 2*c[2]-p[2]); });

    // Reflective symmetry with respect X, Y and Z axis.
    ops.push_back([=](const VectorF& p) { return Vector3F(2*c[0]-p[0], 2*c[1]-p[1],        p[2]); });
    ops.push_back([=](const VectorF& p) { return Vector3F(       p[0], 2*c[1]-p[1], 2*c[2]-p[2]); });
    ops.push_back([=](const VectorF& p) { return Vector3F(2*c[0]-p[0],        p[1], 2*c[2]-p[2]); });

    // Reflective symmetry with respect to center.
    ops.push_back([=](const VectorF& p) { return Vector3F(2*c[0]-p[0], 2*c[1]-p[1], 2*c[2]-p[2]); });

    return ops;
}

template<>
SymmetryOperators::Operators SymmetryOperators::generate_rotational_symmetry_operators<2>(const VectorF& c) {
    assert(c.size() == 2);
    Operators ops;

    ops.push_back([=](const VectorF& p) {
            return Vector2F(c[0]+p[1]-c[1], c[1]-p[0]+c[0]); });
    ops.push_back([=](const VectorF& p) {
            return Vector2F(c[0]-p[1]+c[1], c[1]+p[0]-c[0]); });
    ops.push_back([=](const VectorF& p) {
            return Vector2F(c[0]-p[0]+c[0], c[1]-p[1]+c[1]); });

    return ops;
}

template<>
SymmetryOperators::Operators SymmetryOperators::generate_rotational_symmetry_operators<3>(const VectorF& c) {
    assert(c.size() == 3);
    Operators ops;

    ops.push_back([=](const VectorF& p)
            { return Vector3F(p[0], c[1]+p[2]-c[2], c[2]-p[1]+c[1]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(p[0], c[1]-p[1]+c[1], c[2]-p[2]+c[2]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(p[0], c[1]-p[2]+c[2], c[2]+p[1]-c[1]); });

    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[2]+p[0]-c[0], p[1], c[0]-p[2]+c[2]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[0]-p[0]+c[0], p[1], c[2]-p[2]+c[2]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[2]-p[0]+c[0], p[1], c[0]+p[2]-c[2]); });

    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[0]+p[1]-c[1], c[1]-p[0]+c[0], p[2]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[0]-p[0]+c[0], c[1]-p[1]+c[1], p[2]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[0]-p[1]+c[1], c[1]+p[0]-c[0], p[2]); });

    return ops;
}

SymmetryOperators::SymmetryConnectivity SymmetryOperators::compute_vertex_connectivity(
        const MatrixFr& vertices, SymmetryOperators::Operators& ops) {
    const Float tol = 1e-12;
    const size_t num_vertices = vertices.rows();

    SymmetryConnectivity connectivity(num_vertices);
    for (auto& op : ops) {
        std::vector<bool> visited(num_vertices, false);
        for (size_t i=0; i<num_vertices; i++) {
            if (visited[i]) continue;

            const VectorF& curr_p = vertices.row(i);
            VectorF mapped_p = op(curr_p);
            for (size_t j=i+1; j<num_vertices; j++) {
                const VectorF& other_p = vertices.row(j);
                Float squared_dist = (mapped_p - other_p).squaredNorm();
                if (squared_dist < tol) {
                    visited[i] = true;
                    visited[j] = true;
                    connectivity[i].push_back(j);
                    connectivity[j].push_back(i);
                }
            }
        }
    }

    return connectivity;
}

SymmetryOperators::SymmetryConnectivity SymmetryOperators::compute_edge_connectivity(
        const MatrixFr& vertices,
        const MatrixIr& edges,
        SymmetryOperators::Operators& ops) {
    const Float tol = 1e-12;
    const size_t num_edges = edges.rows();

    SymmetryConnectivity connectivity(num_edges);
    for (auto& op : ops) {
        std::vector<bool> visited(num_edges, false);
        for (size_t i=0; i<num_edges; i++) {
            if (visited[i]) continue;

            const VectorI& curr_e = edges.row(i);
            const VectorF& e0 = vertices.row(curr_e[0]);
            const VectorF& e1 = vertices.row(curr_e[1]);
            VectorF mapped_e0 = op(e0);
            VectorF mapped_e1 = op(e1);

            for (size_t j=i+1; j<num_edges; j++) {
                const VectorI& other_e = edges.row(j);
                const VectorF& other_e0 = vertices.row(other_e[0]);
                const VectorF& other_e1 = vertices.row(other_e[1]);

                Float dist_1 = (mapped_e0 - other_e0).squaredNorm() +
                    (mapped_e1 - other_e1).squaredNorm();
                Float dist_2 = (mapped_e0 - other_e1).squaredNorm() +
                    (mapped_e1 - other_e0).squaredNorm();
                if (dist_1 < tol || dist_2 < tol) {
                    visited[i] = true;
                    visited[j] = true;
                    connectivity[i].push_back(j);
                    connectivity[j].push_back(i);
                }
            }
        }
    }

    return connectivity;
}

VectorI SymmetryOperators::label_connected_components(size_t num_entries,
        const std::vector<std::list<size_t> >& connectivity) {
    size_t count = 0;
    VectorI components = VectorI::Ones(num_entries) * -1;
    for (size_t i=0; i<num_entries; i++) {
        if (components[i] >= 0) continue;

        std::queue<size_t> Q;
        Q.push(i);
        components[i] = count;

        while (!Q.empty()) {
            size_t curr = Q.front();
            Q.pop();
            for (auto neighbor : connectivity[curr]) {
                if (components[neighbor] == -1) {
                    Q.push(neighbor);
                    components[neighbor] = count;
                }
            }
        }
        count++;
    }
    return components;
}

