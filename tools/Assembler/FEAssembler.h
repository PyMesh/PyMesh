#pragma once

#include <string>

#include <Mesh.h>

#include <Math/ZSparseMatrix.h>
#include <Materials/Material.h>
#include <Assembler/FESetting/FESetting.h>

namespace PyMesh {

class FEAssembler {
    public:
        static FEAssembler create(Mesh::Ptr mesh, Material::Ptr material);
        static FEAssembler create_from_name(Mesh::Ptr mesh, const std::string& name);

    public:
        ZSparseMatrix assemble(const std::string& matrix_name);
        void set_material(Material::Ptr material) {
            m_setting->set_material(material);
        }

    private:
        FEAssembler(Mesh::Ptr mesh, const std::string& material_name);
        FEAssembler(Mesh::Ptr mesh, Material::Ptr material);

    private:
        FESetting::Ptr m_setting;
};

}
