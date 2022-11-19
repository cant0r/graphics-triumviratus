#pragma once
#include "Vertex.h"
/**
* WingedEdge Edge definition according to the Wikipedia article https://en.wikipedia.org/wiki/Winged_edge
*/
class Edge {
public:
	/**
	 * Constructor
	 */
	Edge();

	/**
	 * Constructor
	 * @param origin Origin of edge
	 * @param destination Destination of edge
	 */
	Edge(Vertex* origin, Vertex* destination);

	Vertex* mVertOrigin; // Origin Vertex
	Vertex* mVertDest; // Destination Vertex
	Face* mLeftFace; // Left Face
	Face* mRightFace; // Right Face
	Edge* mEdgeLeftCW; // Left Clockwise Edge
	Edge* mEdgeLeftCCW; // Left CounterClockwise Edge
	Edge* mEdgeRightCW; // Right Clockwise Edge
	Edge* mEdgeRightCCW; // Right Counterclockwise edge
};

