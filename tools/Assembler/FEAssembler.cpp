#include "FEAssembler.h"

#include <Assembler/Assemblers/Assembler.h>
#include <Assembler/FESetting/FESetting.h>
#include <Assembler/FESetting/FESettingFactory.h>

using namespace PyMesh;

FEAssembler FEAssembler::create(
        Mesh::Ptr mesh, Material::Ptr material) {
    return FEAssembler(mesh, material);
}

FEAssembler FEAssembler::create_from_name(
        Mesh::Ptr mesh, const std::string& name) {
    return FEAssembler(mesh, name);
}

FEAssembler::FEAssembler(Mesh::Ptr mesh, const std::string& material_name) {
    FESettingFactory factory(mesh);
    factory.with_material(material_name);
    m_setting = factory.create();
}

FEAssembler::FEAssembler(Mesh::Ptr mesh, Material::Ptr material) {
    FESettingFactory factory(mesh);
    factory.with_material(material);
    m_setting = factory.create();
}

ZSparseMatrix FEAssembler::assemble(const std::string& matrix_name) {
    Assembler::Ptr assembler = Assembler::create(matrix_name);
    return assembler->assemble(m_setting);
}
