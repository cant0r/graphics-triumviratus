#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
class WingedEdge
{
public:
    WingedEdge();
    ~WingedEdge();

    /**
     * Populates WingedEdge faces, edges and vertices with appropriate inter-links using provided vertices and faces (vertex indices)
     * @param vertices 3xn array of vertices
     * @param faces 3xt array of vertex indices of each triangular face
     * @return true if success
     */
    bool loadModel(std::vector<glm::vec3> vertices, std::vector<glm::ivec3> faces);

    /**
     * Check whether a model is loaded.
     * @return True if model is loaded. Otherwise false.
     */
    bool isModelLoaded() const;

    /**
     * Returns minimum coordinate in all 3 axis. Used to set camera parameters when a new model is loaded.
     * @return
     */
    glm::vec3 getMinPoint() const;

    /**
     * Returns maximum coordinate in all 3 axis. Used to set camera parameters when a new model is loaded.
     * @return
     */
    glm::vec3 getMaxPoint() const;

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
    // WingedEdge elements live in following arrays
    Vertex* mVertices;
    Edge* mEdges;
    Face* mFaces;

    int mVertexCount;
    int mFaceCount;
    int mEdgeCount;

    glm::vec3 mMinPoint;
    glm::vec3 mMaxPoint;

    // Used to avoid re-calculations
    bool mSmoothedShaded;
    bool mFlatShaded;

    bool mModelLoaded;

    /**
     * Unload mesh.
     */
    void unloadModel();
};

