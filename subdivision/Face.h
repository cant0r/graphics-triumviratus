#pragma once

class Edge;

class Face
{
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

