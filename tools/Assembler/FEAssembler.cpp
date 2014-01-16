#include "FEAssembler.h"

#include <Assembler/Assemblers/Assembler.h>
#include <Assembler/FESetting/FESetting.h>
#include <Assembler/FESetting/FESettingFactory.h>

FEAssembler::FEAssembler(Mesh::Ptr mesh) {
    FESettingFactory factory(mesh);
    factory.with_material("TestMaterial");
    m_setting = factory.create();
}

ZSparseMatrix FEAssembler::assemble(const std::string& matrix_name) {
    Assembler::Ptr assembler = Assembler::create(matrix_name);
    return assembler->assemble(m_setting);
}
