#ifndef PYMESH_MESHSTATISTICS_H
#define PYMESH_MESHSTATISTICS_H

/**
 * A simple wrapper for mesh statistics
 */
class MeshStatistics {

public:
    int nbVertices;
    int nbEdges;
    int nbFacets;

    double minEdgeLength;
    double maxEdgeLength;
    double meanEdgeLength;
    double medianEdgeLength;
    unsigned int nbNullLengthEdge;

    double minAngle;
    double maxAngle;
    double meanAngle;

    double minArea;
    double maxArea;
    double medArea;
    double meanArea;

    double minAltitude;
    double minAspectRatio;
    double maxAspectRatio;
    double meanAspectRatio;

    int numDegeneratedFaces;
    double area;
    double volume;
    bool selfIntersect;
    int genus;

    bool isTriangularMesh;
    bool isClosed;
    bool hasNonManifoldVertices;

    int numComponents;
    int numBorders;
};

#endif //PYMESH_MESHSTATISTICS_H
