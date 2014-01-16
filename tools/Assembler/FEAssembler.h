#pragma once

#include <Mesh.h>

#include <Assembler/Math/ZSparseMatrix.h>
#include <Assembler/FESetting/FESetting.h>

class FEAssembler {
    public:
        FEAssembler(Mesh::Ptr mesh);

    public:
        ZSparseMatrix assemble(const std::string& matrix_name);

    private:
        FESetting::Ptr m_setting;
};
