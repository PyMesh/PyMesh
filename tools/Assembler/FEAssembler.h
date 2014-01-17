#pragma once

#include <string>

#include <Mesh.h>

#include <Assembler/Math/ZSparseMatrix.h>
#include <Assembler/FESetting/FESetting.h>

class FEAssembler {
    public:
        FEAssembler(Mesh::Ptr mesh, const std::string& material_name);

    public:
        ZSparseMatrix assemble(const std::string& matrix_name);

    private:
        FESetting::Ptr m_setting;
};
