/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "Assembler.h"

namespace PyMesh {

class StiffnessAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};

}
