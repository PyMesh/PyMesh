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
#include "Assemblers/RigidMotionAssemblerTest.h"
#include "Elements/ElementsTest.h"
#include "Elements/TetrahedronElementsTest.h"
#include "Elements/TriangleElementsTest.h"
#include "Elements/EdgeElementsTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
