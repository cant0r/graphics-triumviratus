#include "Edge.h"
#include "Vertex.h"

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

Edge::Edge(Vertex* origin, Vertex* destination) {
	mVertOrigin = origin;
	mVertDest = destination;
}