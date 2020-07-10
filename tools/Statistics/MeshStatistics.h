#ifndef PYMESH_MESHSTATISTICS_H
#define PYMESH_MESHSTATISTICS_H

/**
 * A simple wrapper for mesh statistics
 */
class MeshStatistics {

    int nbVertices;
    int nbEdges;
    int nbFacets;

    double minEdgeLength, maxEdgeLength, meanEdgeLength, medianEdgeLength;
    unsigned int nbNullLengthEdge;

    double minAngle, maxAngle, meanAngle;

    double minArea, maxArea, medArea, meanArea;
    double minAltitude, minAspectRatio, maxAspectRatio, meanAspectRatio;

    int numDegeneratedFaces;
    double area;
    double volume;
    bool selfIntersect;
    int genus;

    bool isTriangularMesh;
    bool isClosed;
    bool hasNonManifoldVertices;

    int numComponents, numBorders;

};

MeshStatistics computeMeshStatistics(Polyhedron &polyhedron);

#endif //PYMESH_MESHSTATISTICS_H
