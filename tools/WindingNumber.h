#pragma once

#include <EigenTypedef.h>

class Mesh;

class WindingNumber {
    public:
        WindingNumber(Mesh& mesh) : m_mesh(mesh) {}

    public:
        VectorF compute(const VectorF& pts);

    private:
        Mesh& m_mesh;
};
