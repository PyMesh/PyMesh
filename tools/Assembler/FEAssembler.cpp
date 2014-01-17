#include "FEAssembler.h"

#include <Assembler/Assemblers/Assembler.h>
#include <Assembler/FESetting/FESetting.h>
#include <Assembler/FESetting/FESettingFactory.h>

FEAssembler::FEAssembler(Mesh::Ptr mesh, const std::string& material_name) {
    FESettingFactory factory(mesh);
    factory.with_material(material_name);
    m_setting = factory.create();
}

ZSparseMatrix FEAssembler::assemble(const std::string& matrix_name) {
    Assembler::Ptr assembler = Assembler::create(matrix_name);
    return assembler->assemble(m_setting);
}
