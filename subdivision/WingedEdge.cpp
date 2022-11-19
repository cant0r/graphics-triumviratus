#include "WingedEdge.h"

WingedEdge::WingedEdge() {
    mVertices = nullptr;
    mFaces = nullptr;
    mEdges = nullptr;
    mMinPoint = glm::vec3(0, 0, 0);
    mMaxPoint = glm::vec3(0, 0, 0);
    mVertexCount = 0;
    mFaceCount = 0;
    mEdgeCount = 0;
    mSmoothedShaded = false;
    mFlatShaded = false;
    mModelLoaded = false;
}

void WingedEdge::unloadModel() {
    if (!mModelLoaded)
        return;

    // Reset state variables
    mMinPoint = glm::vec3(0, 0, 0);
    mMaxPoint = glm::vec3(0, 0, 0);

    mVertexCount = 0;
    mFaceCount = 0;
    mEdgeCount = 0;

    mSmoothedShaded = false;
    mFlatShaded = false;

    delete[] mVertices;
    delete[] mEdges;
    delete[] mFaces;

    mVertices = nullptr;
    mEdges = nullptr;
    mFaces = nullptr;

    mModelLoaded = false;

}

/**
 * Populates WingedEdge faces, edges and vertices with appropriate inter-links using provided vertices and faces (vertex indices)
 * @param vertices 3xn array of vertices
 * @param faces 3xt array of vertex indices of each triangular face
 * @return true if success
 */
bool WingedEdge::loadModel(std::vector<glm::vec3> vertices, std::vector<glm::ivec3> faces) {
    unloadModel();

    mVertexCount = vertices.size();
    mFaceCount = faces.size();
    mEdgeCount = mFaceCount * 3 / 2;

    mVertices = new Vertex[mVertexCount];
    mEdges = new Edge[mFaceCount * 3 / 2];
    mFaces = new Face[mFaceCount];

    // Used to locate an edge using a vertex pair. Think of this as a hash map
    Edge*** vertexPairEdgeArray = new Edge * *[mVertexCount];
    for (int i = 0; i < mVertexCount; i++) {
        vertexPairEdgeArray[i] = new Edge * [mVertexCount];
        for (int j = 0; j < mVertexCount; j++) {
            vertexPairEdgeArray[i][j] = nullptr;
        }
    }

    // load vertices
    for (int i = 0; i < mVertexCount; i++) {
        mVertices[i].setPosition(vertices[i].x, vertices[i].y, vertices[i].z);

        // update dimensions
        for (int j = 0; j < 3; j++)
        {
            if (mVertices[i].getPosition()[j] < mMinPoint[j]) {
                mMinPoint[j] = mVertices[i].getPosition()[j];
            }
            if (mVertices[i].getPosition()[j] > mMaxPoint[j]) {
                mMaxPoint[j] = mVertices[i].getPosition()[j];
            }
        }
    }

    int edgeCount = 0;
    // Loop through faces and populate edges. Fill LeftFace and RightFace.
    for (int f = 0; f < mFaceCount; f++)
    {
        Edge* edgeBank[] = { nullptr, nullptr, nullptr }; //keep track of 3 edges belonging to this face
        for (int k = 0; k < 3; k++)
        {
            int v1_index = faces[f][k];
            int v2_index = faces[f][(k + 1) % 3];
            // v1 => v2 is counter clockwise direction
            // v2 => v1 is clockwise direction
            // v2 => v1 is my edge in clockwise direction. But need to check whether v1 => v2 is already acquired by another face in clockwise direction
            if (vertexPairEdgeArray[v1_index][v2_index] == nullptr) {
                // OK. the edge v1 => v2 does not exist. I can create edge v2 => v1.
                vertexPairEdgeArray[v2_index][v1_index] = &(mEdges[edgeCount]);
                mEdges[edgeCount].mVertOrigin = &mVertices[v2_index];
                mEdges[edgeCount].mVertDest = &mVertices[v1_index];
                // I am the right face.
                mEdges[edgeCount].mRightFace = &mFaces[f];
                if (k == 0)
                {
                    mFaces[f].setEdge(&(mEdges[edgeCount]));
                }
                edgeBank[k] = &(mEdges[edgeCount]);

                // Check whether two end nodes has an edge. If not, set this edge
                if (mVertices[v1_index].getEdge() == nullptr)
                    mVertices[v1_index].setEdge(&(mEdges[edgeCount]));

                if (mVertices[v2_index].getEdge() == nullptr)
                    mVertices[v2_index].setEdge(&(mEdges[edgeCount]));

                edgeCount++;
            }
            else {
                // Edge v1 => v2 already exists. I cant create v2=>v1. Instead, I should be the other face
                vertexPairEdgeArray[v1_index][v2_index]->mLeftFace = &mFaces[f];
                if (k == 0)
                {
                    mFaces[f].setEdge(vertexPairEdgeArray[v1_index][v2_index]);
                }
                edgeBank[k] = vertexPairEdgeArray[v1_index][v2_index];
            }
        }

        assert(edgeBank[0] != nullptr);
        assert(edgeBank[1] != nullptr);
        assert(edgeBank[2] != nullptr);

        //Now to fill Adjacent edges
        for (int k = 0; k < 3; k++)
        {
            int v1_index = faces[f][k];
            int v2_index = faces[f][(k + 1) % 3];
            // v2 => v1 is clockwise direction
            if (vertexPairEdgeArray[v2_index][v1_index] != nullptr)
            {
                assert(vertexPairEdgeArray[v1_index][v2_index] == nullptr);
                // I have set v2=>v1. I am the right face. k is anticlockwise.
                int c = k - 1;
                if (c < 0) c = 2;
                vertexPairEdgeArray[v2_index][v1_index]->mEdgeRightCW = edgeBank[c];
                vertexPairEdgeArray[v2_index][v1_index]->mEdgeRightCCW = edgeBank[(k + 1) % 3];
            }
            else if (vertexPairEdgeArray[v1_index][v2_index] != nullptr) {
                assert(vertexPairEdgeArray[v2_index][v1_index] == nullptr);
                // I have joined to v1 => v2 face already in existence. So, I am the left face. k is anticlockwise.
                vertexPairEdgeArray[v1_index][v2_index]->mEdgeLeftCCW = edgeBank[(k + 1) % 3];

                int c = k - 1;
                if (c < 0) c = 2;
                vertexPairEdgeArray[v1_index][v2_index]->mEdgeLeftCW = edgeBank[c];
            }
        }


    }

    for (int i = 0; i < mVertexCount; i++) {
        delete[] vertexPairEdgeArray[i];
    }
    delete[] vertexPairEdgeArray;

    mModelLoaded = true;
    return true;
}

glm::vec3 WingedEdge::getMinPoint() const {
    
    return mMinPoint;
}

glm::vec3 WingedEdge::getMaxPoint() const {
    
    return mMaxPoint;
}

WingedEdge::~WingedEdge() {
    unloadModel();
}

Vertex* WingedEdge::getVertices() const {
    return mVertices;
}

Edge* WingedEdge::getEdges() const {
    return mEdges;
}

Face* WingedEdge::getFaces() const {
    return mFaces;
}

int WingedEdge::getVertexCount() const {
    return mVertexCount;
}

int WingedEdge::getEdgeCount() const {
    return mEdgeCount;
}

int WingedEdge::getFaceCount() const {
    return mFaceCount;
}

void WingedEdge::allocateVertices(int v) {
    delete[] mVertices;
    mVertexCount = v;
    mVertices = new Vertex[mVertexCount];
}
void WingedEdge::allocateEdges(int e) {
    delete[] mEdges;
    mEdgeCount = e;
    mEdges = new Edge[mEdgeCount];
}
void WingedEdge::allocateFaces(int f) {
    delete[] mFaces;
    mFaceCount = f;
    mFaces = new Face[mFaceCount];
}

bool WingedEdge::isModelLoaded() const {
    return mModelLoaded;
}