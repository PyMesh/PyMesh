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
