#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MeshTest.h"
#include "MeshFactoryTest.h"
#include "IO/OBJParserTest.h"
#include "IO/OBJWriterTest.h"
#include "IO/MSHParserTest.h"
#include "IO/MSHWriterTest.h"
#include "IO/PLYParserTest.h"
#include "IO/PLYWriterTest.h"
#include "Math/ZSparseMatrixTest.h"
#include "Misc/TripletTest.h"
#include "Misc/MultipletTest.h"
#include "Misc/HashGridTest.h"
#include "Misc/HashKeyTest.h"
#include "Attributes/FaceCentroidAttributeTest.h"
#include "Attributes/FaceCircumCenterAttributeTest.h"
#include "Attributes/FaceIndexAttributeTest.h"
#include "Attributes/FaceVoronoiAreaAttributeTest.h"
#include "Attributes/VoxelVolumeAttributeTest.h"
#include "Attributes/VoxelCentroidAttributeTest.h"
#include "Attributes/VoxelIndexAttributeTest.h"
#include "Attributes/VertexIndexAttributeTest.h"
#include "Attributes/VertexGaussianCurvatureAttributeTest.h"
#include "Attributes/VertexNormalAttributeTest.h"
#include "Attributes/VertexMeanCurvatureAttributeTest.h"
#include "Attributes/VertexLaplacianAttributeTest.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
