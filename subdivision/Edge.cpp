#include "Edge.h"

Edge::Edge() {
    mVertDest = nullptr;
    mVertOrigin = nullptr;
    mEdgeLeftCCW = nullptr;
    mEdgeLeftCW = nullptr;
    mEdgeRightCCW = nullptr;
    mEdgeRightCW = nullptr;
    mLeftFace = nullptr;
    mRightFace = nullptr;
}

Edge::Edge(Vertex* origin, Vertex* destination) : Edge() {
    mVertOrigin = origin;
    mVertDest = destination;
}