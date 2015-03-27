/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Mesh/TriangleMeshTest.h"
#include "Mesh/TetrahedronMeshTest.h"
#include "ShapeFunctions/ShapeFunctionTest.h"
#include "ShapeFunctions/IntegratorTest.h"
#include "ShapeFunctions/FEBasisTest.h"
#include "FESetting/FESettingTest.h"
#include "Assemblers/StiffnessAssemblerTest.h"
#include "Assemblers/MassAssemblerTest.h"
#include "Assemblers/LumpedMassAssemblerTest.h"
#include "Assemblers/LaplacianAssemblerTest.h"
#include "Assemblers/DisplacementStrainAssemblerTest.h"
#include "Assemblers/ElasticityTensorAssemblerTest.h"
#include "Assemblers/EngineerStrainStressAssemblerTest.h"
#include "Assemblers/RigidMotionAssemblerTest.h"
#include "Elements/ElementsTest.h"
#include "Elements/TetrahedronElementsTest.h"
#include "Elements/TriangleElementsTest.h"
#include "Elements/EdgeElementsTest.h"
#include "Materials/ElementWiseIsotropicMaterialTest.h"
#include "Materials/ElementWiseOrthotropicMaterialTest.h"
#include "Materials/ElementWiseSymmetricMaterialTest.h"
#include "Materials/IsotropicMaterialTest.h"
#include "Materials/MaterialTest.h"
#include "Materials/OrthotropicMaterialTest.h"
#include "Materials/PeriodicMaterialTest.h"
#include "Materials/SymmetricMaterialTest.h"
#include "Materials/UniformMaterialTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
