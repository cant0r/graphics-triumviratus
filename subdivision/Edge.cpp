#include "Edge.h"


Edge::Edge()
{
	edgeLeftCCW = nullptr;
	edgeRightCCW = nullptr;
	edgeLeftCW = nullptr;
	edgeRightCW = nullptr;
	leftFace = nullptr;
	rightFace = nullptr;
	vertOrigin = nullptr;
	vertDest = nullptr;
}

Edge::Edge(Vertex* origin, Vertex* destination) : Edge() 
{
	vertOrigin = origin;
	vertDest = destination;
}