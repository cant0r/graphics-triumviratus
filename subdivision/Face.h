#pragma once
#include "Edge.h"
/**
* WingedEdge Face definition according to the Wikipedia article https://en.wikipedia.org/wiki/Winged_edge
*/
class Face {
private:
	Edge* mEdge; //First edge of face
public:
	Face();
	/**
	 * Get an edge adjacent to face
	 * @return
	 */
	Edge* getEdge() const;

	/**
	 * Set an edge adjacent to face
	 * @param nEdge
	 */
	void setEdge(Edge* nEdge);
};
