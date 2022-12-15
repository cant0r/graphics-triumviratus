#pragma once
#include <vector>
#include <glm/ext.hpp>
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"

class WEMesh
{
public:
    WEMesh();
    ~WEMesh();

    bool loadModel(std::vector<glm::vec3>& vertices, std::vector<glm::ivec3>& faces);
    bool isModelLoaded() const;

    
    Vertex* getVertices() const;
    Edge* getEdges() const;
    Face* getFaces() const;

    int getVertexCount() const;
    int getEdgeCount() const;
    int getFaceCount() const;

    void allocateVertices(int v);
    void allocateEdges(int e);
    void allocateFaces(int f);

private:
    Vertex* vertices;
    Edge* edges;
    Face* faces;

    int vertexCount;
    int faceCount;
    int edgeCount;

    bool modelLoaded;

    void unloadModel();
};

