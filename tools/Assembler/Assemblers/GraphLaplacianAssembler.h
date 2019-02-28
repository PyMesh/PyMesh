/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */
#pragma once
#include "Assembler.h"

namespace PyMesh {

class GraphLaplacianAssembler : public Assembler {
    public:
        virtual ZSparseMatrix assemble(FESettingPtr setting);
};

}
