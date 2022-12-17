#include "LoopSubdivision.h"
#include <iostream>

#include <math.h>
#define M_PI 3.14159265358979323846  /* pi */

/**
 * Each edge of source edge is represented by two edges in the resultant mesh of loop subdivision.
 * The extended edge data structure is used to maintain relationship between each edge in source mesh and two newly added
 * edges in target mesh
 */
class ExtendedEdge
{
public:
	Edge* originalEdge;
	Edge* newEdgeS;
	Edge* newEdgeE;
	bool added;

	ExtendedEdge()
	{
		originalEdge = nullptr;
		newEdgeE = nullptr;
		newEdgeS = nullptr;
		added = false;
	}
};

/**
 * Each triangle of original mesh is represented by 4 triangles in the subdivided mesh. Extended face is used to link
 * each face of original mesh with 4 faces in resultant mesh.
 */
class ExtendedFace {
public:
	Face* originalFace;
	ExtendedEdge* extendedEdge1;
	ExtendedEdge* extendedEdge2;
	ExtendedEdge* extendedEdge3;

	Edge* internalEdge12; // From midpoint of external edge 1 to midpoint of external edge 2
	Edge* internalEdge23;
	Edge* internalEdge31;

	bool faceAdded;

	ExtendedFace()
	{
		originalFace = nullptr;
		extendedEdge1 = extendedEdge2 = extendedEdge3 = nullptr;
		internalEdge12 = internalEdge23 = internalEdge31 = nullptr;
		faceAdded = false;
	}
};

/**
 * Rerrieve vertex shared by two edges e1 and e2
 * @param e1
 * @param e2
 * @return
 */
Vertex* getCommonVertex(const Edge* e1, const Edge* e2)
{
	if (e1->vertOrigin == e2->vertOrigin)
		return e1->vertOrigin;
	else if (e1->vertOrigin == e2->vertDest)
		return e1->vertOrigin;
	else if (e1->vertDest == e2->vertOrigin)
		return e1->vertDest;
	else if (e1->vertDest == e2->vertDest)
		return e1->vertDest;
	assert(false);
	return nullptr;
}

/**
 * Apply vertex rule of loop subdivision to the specified vertex
 * @param sourceMesh Mesh used to read neighbouring vertices
 * @param targetMesh Mesh that gets updated with the new vertex rule
 * @param vertexIndex Index of vertex to update
 */
void applyVertexRule(const WEMesh* sourceMesh, WEMesh* targetMesh,
	int vertexIndex)
{
	//Apply Vertex rule

	// Accumulate coordinates of neighbouring vertices
	float neighbourX = 0;
	float neighbourY = 0;
	float neighbourZ = 0;

	Vertex* currentVertex = &(sourceMesh->getVertices()[vertexIndex]);
	Edge* startEdge = currentVertex->getEdge();
	// Walk around clockwise and count edges and sum up neighbours coordinates
	Edge* currentEdge = nullptr;
	int adjacentEdgeCount = 0;
	if (startEdge->vertOrigin == currentVertex)
	{
		if (startEdge->edgeRightCW->vertOrigin == currentVertex ||
			startEdge->edgeRightCW->vertDest == currentVertex)
		{
			currentEdge = (startEdge->edgeRightCW);
			if (startEdge->edgeRightCW->vertOrigin == currentVertex)
			{
				neighbourX += startEdge->edgeRightCW->vertDest->getX();
				neighbourY += startEdge->edgeRightCW->vertDest->getY();
				neighbourZ += startEdge->edgeRightCW->vertDest->getZ();
			}
			else
			{
				neighbourX += startEdge->edgeRightCW->vertOrigin->getX();
				neighbourY += startEdge->edgeRightCW->vertOrigin->getY();
				neighbourZ += startEdge->edgeRightCW->vertOrigin->getZ();
			}
		}
		else if (startEdge->edgeRightCCW->vertOrigin == currentVertex ||
			startEdge->edgeRightCCW->vertDest == currentVertex)
		{
			currentEdge = (startEdge->edgeRightCCW);
			if (startEdge->edgeRightCCW->vertOrigin == currentVertex)
			{
				neighbourX += startEdge->edgeRightCCW->vertDest->getX();
				neighbourY += startEdge->edgeRightCCW->vertDest->getY();
				neighbourZ += startEdge->edgeRightCCW->vertDest->getZ();
			}
			else
			{
				neighbourX += startEdge->edgeRightCCW->vertOrigin->getX();
				neighbourY += startEdge->edgeRightCCW->vertOrigin->getY();
				neighbourZ += startEdge->edgeRightCCW->vertOrigin->getZ();
			}
		}
		else
		{
			assert(false);
		}
	}
	else if (startEdge->vertDest == currentVertex)
	{
		if (startEdge->edgeLeftCW->vertOrigin == currentVertex ||
			startEdge->edgeLeftCW->vertDest == currentVertex)
		{
			currentEdge = (startEdge->edgeLeftCW);
			if (startEdge->edgeLeftCW->vertOrigin == currentVertex)
			{
				neighbourX += startEdge->edgeLeftCW->vertDest->getX();
				neighbourY += startEdge->edgeLeftCW->vertDest->getY();
				neighbourZ += startEdge->edgeLeftCW->vertDest->getZ();
			}
			else
			{
				neighbourX += startEdge->edgeLeftCW->vertOrigin->getX();
				neighbourY += startEdge->edgeLeftCW->vertOrigin->getY();
				neighbourZ += startEdge->edgeLeftCW->vertOrigin->getZ();
			}
		}
		else if (startEdge->edgeLeftCCW->vertOrigin == currentVertex ||
			startEdge->edgeLeftCCW->vertDest == currentVertex)
		{
			currentEdge = (startEdge->edgeLeftCCW);
			if (startEdge->edgeLeftCCW->vertOrigin == currentVertex)
			{
				neighbourX += startEdge->edgeLeftCCW->vertDest->getX();
				neighbourY += startEdge->edgeLeftCCW->vertDest->getY();
				neighbourZ += startEdge->edgeLeftCCW->vertDest->getZ();
			}
			else
			{
				neighbourX += startEdge->edgeLeftCCW->vertOrigin->getX();
				neighbourY += startEdge->edgeLeftCCW->vertOrigin->getY();
				neighbourZ += startEdge->edgeLeftCCW->vertOrigin->getZ();
			}
		}
		else
		{
			assert(false);
		}
	}
	else
	{
		assert(false);
	}
	assert(currentEdge != startEdge);
	adjacentEdgeCount += 1;
	while (currentEdge != startEdge)
	{ // Keep walking until we end up at the edge we started
		adjacentEdgeCount += 1;
		if (currentEdge->vertOrigin == currentVertex)
		{
			if (currentEdge->edgeRightCW->vertOrigin == currentVertex ||
				currentEdge->edgeRightCW->vertDest == currentVertex)
			{
				currentEdge = (currentEdge->edgeRightCW);
				if (currentEdge->edgeRightCW->vertOrigin == currentVertex)
				{
					neighbourX += currentEdge->edgeRightCW->vertDest->getX();
					neighbourY += currentEdge->edgeRightCW->vertDest->getY();
					neighbourZ += currentEdge->edgeRightCW->vertDest->getZ();
				}
				else
				{
					neighbourX += currentEdge->edgeRightCW->vertOrigin->getX();
					neighbourY += currentEdge->edgeRightCW->vertOrigin->getY();
					neighbourZ += currentEdge->edgeRightCW->vertOrigin->getZ();
				}
			}
			else if (currentEdge->edgeRightCCW->vertOrigin == currentVertex ||
				currentEdge->edgeRightCCW->vertDest == currentVertex)
			{
				currentEdge = (currentEdge->edgeRightCCW);
				if (currentEdge->edgeRightCCW->vertOrigin == currentVertex)
				{
					neighbourX += currentEdge->edgeRightCCW->vertDest->getX();
					neighbourY += currentEdge->edgeRightCCW->vertDest->getY();
					neighbourZ += currentEdge->edgeRightCCW->vertDest->getZ();
				}
				else
				{
					neighbourX += currentEdge->edgeRightCCW->vertOrigin->getX();
					neighbourY += currentEdge->edgeRightCCW->vertOrigin->getY();
					neighbourZ += currentEdge->edgeRightCCW->vertOrigin->getZ();
				}
			}
			else
			{
				assert(false);
			}
		}
		else if (currentEdge->vertDest == currentVertex)
		{
			if (currentEdge->edgeLeftCW->vertOrigin == currentVertex ||
				currentEdge->edgeLeftCW->vertDest == currentVertex)
			{
				currentEdge = (currentEdge->edgeLeftCW);
				if (currentEdge->edgeLeftCW->vertOrigin == currentVertex)
				{
					neighbourX += currentEdge->edgeLeftCW->vertDest->getX();
					neighbourY += currentEdge->edgeLeftCW->vertDest->getY();
					neighbourZ += currentEdge->edgeLeftCW->vertDest->getZ();
				}
				else
				{
					neighbourX += currentEdge->edgeLeftCW->vertOrigin->getX();
					neighbourY += currentEdge->edgeLeftCW->vertOrigin->getY();
					neighbourZ += currentEdge->edgeLeftCW->vertOrigin->getZ();
				}
			}
			else if (currentEdge->edgeLeftCCW->vertOrigin == currentVertex ||
				currentEdge->edgeLeftCCW->vertDest == currentVertex)
			{
				currentEdge = (currentEdge->edgeLeftCCW);
				if (currentEdge->edgeLeftCCW->vertOrigin == currentVertex)
				{
					neighbourX += currentEdge->edgeLeftCCW->vertDest->getX();
					neighbourY += currentEdge->edgeLeftCCW->vertDest->getY();
					neighbourZ += currentEdge->edgeLeftCCW->vertDest->getZ();
				}
				else
				{
					neighbourX += currentEdge->edgeLeftCCW->vertOrigin->getX();
					neighbourY += currentEdge->edgeLeftCCW->vertOrigin->getY();
					neighbourZ += currentEdge->edgeLeftCCW->vertOrigin->getZ();
				}
			}
			else
			{
				assert(false);
			}
		}
		else
		{
			assert(false);
		}
	}

	std::cout << "Adjacent Edge Count " << adjacentEdgeCount << std::endl;
	std::cout << "Neighbour " << neighbourX << " " << neighbourY << " " << neighbourZ << std::endl;

	float k = adjacentEdgeCount;

	// Beta calculation
	float beta = 1.0 / k * (5.0 / 8.0 - ((3.0 / 8.0 + 1.0 / 4.0 * cos(2 * M_PI / k)) *
		(3.0 / 8.0 + 1.0 / 4.0 * cos(2 * M_PI / k))));
	//        beta = 0.05;
	float myX = sourceMesh->getVertices()[vertexIndex].getX();
	float myY = sourceMesh->getVertices()[vertexIndex].getY();
	float myZ = sourceMesh->getVertices()[vertexIndex].getZ();

	float newX = neighbourX * beta + myX * (1 - k * beta);
	float newY = neighbourY * beta + myY * (1 - k * beta);
	float newZ = neighbourZ * beta + myZ * (1 - k * beta);

	targetMesh->getVertices()[vertexIndex].setPosition(newX, newY, newZ);
	//End of Vertex Rule
}

/**
 * Apply edge rule of loop subdivision to a specified edge.
 * @param targetMesh Mesh being updated.
 * @param addedVertexCount Number of vertices added to the target mesh so far.
 * @param edgesCW Edges in clockwise order.
 * @param e Index of specified edge in edgesCW
 */
void applyEdgeRule(const WEMesh* targetMesh, int addedVertexCount, Edge* const* edgesCW,
	int e)
{// Apply Edge Rule
	double newX = edgesCW[e]->vertOrigin->getX() * 3.0 / 8.0 + edgesCW[e]->vertDest->getX() * 3.0 / 8.0;
	double newY = edgesCW[e]->vertOrigin->getY() * 3.0 / 8.0 + edgesCW[e]->vertDest->getY() * 3.0 / 8.0;
	double newZ = edgesCW[e]->vertOrigin->getZ() * 3.0 / 8.0 + edgesCW[e]->vertDest->getZ() * 3.0 / 8.0;

	if (edgesCW[e]->edgeRightCW->vertOrigin == edgesCW[e]->vertOrigin ||
		edgesCW[e]->edgeRightCW->vertOrigin == edgesCW[e]->vertDest)
	{
		newX += edgesCW[e]->edgeRightCW->vertDest->getX() * 1.0 / 8.0;
		newY += edgesCW[e]->edgeRightCW->vertDest->getY() * 1.0 / 8.0;
		newZ += edgesCW[e]->edgeRightCW->vertDest->getZ() * 1.0 / 8.0;
	}
	else if (edgesCW[e]->edgeRightCW->vertDest == edgesCW[e]->vertOrigin ||
		edgesCW[e]->edgeRightCW->vertDest == edgesCW[e]->vertDest)
	{
		newX += edgesCW[e]->edgeRightCW->vertOrigin->getX() * 1.0 / 8.0;
		newY += edgesCW[e]->edgeRightCW->vertOrigin->getY() * 1.0 / 8.0;
		newZ += edgesCW[e]->edgeRightCW->vertOrigin->getZ() * 1.0 / 8.0;
	}
	else
	{
		assert(false);
	}

	if (edgesCW[e]->edgeLeftCW->vertOrigin == edgesCW[e]->vertOrigin ||
		edgesCW[e]->edgeLeftCW->vertOrigin == edgesCW[e]->vertDest)
	{
		newX += edgesCW[e]->edgeLeftCW->vertDest->getX() * 1.0 / 8.0;
		newY += edgesCW[e]->edgeLeftCW->vertDest->getY() * 1.0 / 8.0;
		newZ += edgesCW[e]->edgeLeftCW->vertDest->getZ() * 1.0 / 8.0;
	}
	else if (edgesCW[e]->edgeLeftCW->vertDest == edgesCW[e]->vertOrigin ||
		edgesCW[e]->edgeLeftCW->vertDest == edgesCW[e]->vertDest)
	{
		newX += edgesCW[e]->edgeLeftCW->vertOrigin->getX() * 1.0 / 8.0;
		newY += edgesCW[e]->edgeLeftCW->vertOrigin->getY() * 1.0 / 8.0;
		newZ += edgesCW[e]->edgeLeftCW->vertOrigin->getZ() * 1.0 / 8.0;
	}
	else
	{
		assert(false);
	}

	targetMesh->getVertices()[addedVertexCount].setPosition(newX, newY, newZ);
	// End of Edge Rule
}

/**
 * Update vertices copied from the source mesh
 * @param sourceMesh Source mesh
 * @param useVertexRule Specify true to apply vertex rule.
 * @param targetMesh Target mesh the method updates.
 */
void updateVertices(const WEMesh* sourceMesh, bool useVertexRule,
	WEMesh* targetMesh)
{// Apply vertex rule
	for (int i = 0; i < sourceMesh->getVertexCount(); i++)
	{
		if (useVertexRule)
		{
			applyVertexRule(sourceMesh, targetMesh, i);
		}
		else
		{
			// Just copy forward vertices of vertex rule is not applied
			targetMesh->getVertices()[i].setPosition(sourceMesh->getVertices()[i].getPosition());
		}
	}
}

/**
 * Subdivide edges copied from the source mesh
 * @param sourceMesh Source mesh
 * @param useEdgeRule Specify true to apply edge rule
 * @param targetMesh Target mesh being updated
 * @param extendedEdges The array of extended edges is populated with the relationship between an original edge and a new edge pair.
 * @param addedVertexCount The vertex count is updated.
 * @param addedEdgeCount The edge count is updated.
 */
void subdivideEdges(const WEMesh* sourceMesh, bool useEdgeRule,
	WEMesh* targetMesh, ExtendedEdge* extendedEdges,
	int& addedVertexCount,
	int& addedEdgeCount)
{
	for (int f = 0; f < sourceMesh->getFaceCount(); f++)
	{
		// identify 3 edges of the face in CW order
		Edge* e1 = sourceMesh->getFaces()[f].getEdge();
		Edge* e2 = nullptr;
		Edge* e3 = nullptr;
		if (&(sourceMesh->getFaces()[f]) == e1->rightFace)
		{
			e2 = e1->edgeRightCW;
			e3 = e1->edgeRightCCW;
		}
		else if (&(sourceMesh->getFaces()[f]) == e1->leftFace)
		{
			e2 = e1->edgeLeftCW;
			e3 = e1->edgeLeftCCW;
		}
		else
		{
			assert(false);
		}
		// Create edge vertices
		// Identify three edges in CW orientation
		Edge* edgesCW[] = { e1, e2, e3 };
		for (int e = 0; e < 3; e++)
		{
			if (!(extendedEdges[edgesCW[e] - sourceMesh->getEdges()].added))
			{
				// The edge has not divided yet. Lets divide it and add it

				if (useEdgeRule)
				{
					applyEdgeRule(targetMesh, addedVertexCount, edgesCW, e);
				}
				else
				{
					// Just take average of two vertices of edge rule is not applied
					targetMesh->getVertices()[addedVertexCount].setPosition(
						edgesCW[e]->vertOrigin->getPosition() * 0.5f + edgesCW[e]->vertDest->getPosition() * 0.5f);
				}

				targetMesh->getEdges()[addedEdgeCount].vertOrigin = &(targetMesh->getVertices()[
					edgesCW[e]->vertOrigin - sourceMesh->getVertices()]);
				targetMesh->getEdges()[addedEdgeCount].vertDest = &(targetMesh->getVertices()[addedVertexCount]);
				targetMesh->getVertices()[addedVertexCount].setEdge(&targetMesh->getEdges()[addedEdgeCount]);
				if (targetMesh->getEdges()[addedEdgeCount].vertOrigin->getEdge() == nullptr)
				{
					targetMesh->getEdges()[addedEdgeCount].vertOrigin->setEdge(
						&targetMesh->getEdges()[addedEdgeCount]);
				}

				targetMesh->getEdges()[addedEdgeCount + 1].vertOrigin = &(targetMesh->getVertices()[addedVertexCount]);
				targetMesh->getEdges()[addedEdgeCount + 1].vertDest = &(targetMesh->getVertices()[
					edgesCW[e]->vertDest - sourceMesh->getVertices()]);
				if (targetMesh->getEdges()[addedEdgeCount + 1].vertDest->getEdge() == nullptr)
				{
					targetMesh->getEdges()[addedEdgeCount + 1].vertDest->setEdge(
						&targetMesh->getEdges()[addedEdgeCount + 1]);
				}

				extendedEdges[edgesCW[e] - sourceMesh->getEdges()].added = true;
				extendedEdges[edgesCW[e] - sourceMesh->getEdges()].originalEdge = edgesCW[e];
				extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeS = &(targetMesh->getEdges()[addedEdgeCount]);
				extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeE = &(targetMesh->getEdges()[addedEdgeCount +
					1]);

				addedVertexCount++;
				addedEdgeCount += 2;

				std::cout << "Divided edge [" << edgesCW[e]->vertOrigin->getX() << " "
					<< edgesCW[e]->vertOrigin->getY() << " " << edgesCW[e]->vertOrigin->getZ() <<
					"<< - >>" << edgesCW[e]->vertDest->getX() << " " << edgesCW[e]->vertDest->getY() << " "
					<< edgesCW[e]->vertDest->getZ() <<
					"] as [" << extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeS->vertOrigin->getX()
					<< ", " << extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeS->vertOrigin->getY() <<
					", " << extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeS->vertOrigin->getZ()
					<< " << >> " << extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeS->vertDest->getX()
					<< ", "
					<< extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeS->vertDest->getY() << ", "
					<< extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeS->vertDest->getZ() << "] and ["
					<<
					extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeE->vertOrigin->getX() << ", "
					<< extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeE->vertOrigin->getY() <<
					", " << extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeE->vertOrigin->getZ()
					<< " << >> " << extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeE->vertDest->getX()
					<< ", "
					<< extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeE->vertDest->getY() << ", "
					<< extendedEdges[edgesCW[e] - sourceMesh->getEdges()].newEdgeE->vertDest->getZ()
					<< std::endl;

			}
		}

	}
	for (int _e = 0; _e < sourceMesh->getEdgeCount(); _e++)
	{
		assert(extendedEdges[_e].added == true);
	}
	for (int _v = 0; _v < addedVertexCount; _v++)
	{
		assert(targetMesh->getVertices()[_v].getEdge() != nullptr);
	}

}

/**
 * Subdivide faces, adding the internal edges.
 * @param sourceMesh Source mesh
 * @param targetMesh Target mesh being updated
 * @param extendedFaces The array of extended faces is populated with the relationship between an original face and the 4 triangles in the new subdivided face.
 * @param extendedEdges The array of extended edges is populated with the relationship between an original edge and a new edge pair.
 * @param addedEdgeCount Added edges count is updated.
 */
void subdivideFaces(const WEMesh* sourceMesh, WEMesh* targetMesh,
	ExtendedFace* extendedFaces, ExtendedEdge* extendedEdges,
	int& addedEdgeCount) {
	//Now all the vertices are in place. Now lets subdivide faces, adding the internal edges.
	for (int f = 0; f < sourceMesh->getFaceCount(); f++)
	{
		// identify 3 edges of the face in CW order
		Edge* e1 = sourceMesh->getFaces()[f].getEdge();
		Edge* e2 = nullptr;
		Edge* e3 = nullptr;
		if (&(sourceMesh->getFaces()[f]) == e1->rightFace)
		{
			e2 = e1->edgeRightCW;
			e3 = e1->edgeRightCCW;
		}
		else if (&(sourceMesh->getFaces()[f]) == e1->leftFace)
		{
			e2 = e1->edgeLeftCW;
			e3 = e1->edgeLeftCCW;
		}
		else
		{
			assert(false);
		}
		Edge* edgesCW[] = { e1, e2, e3 };
		ExtendedFace* currentExtendedFace = &(extendedFaces[f]);
		currentExtendedFace->extendedEdge1 = &(extendedEdges[e1 - sourceMesh->getEdges()]);
		currentExtendedFace->extendedEdge2 = &(extendedEdges[e2 - sourceMesh->getEdges()]);
		currentExtendedFace->extendedEdge3 = &(extendedEdges[e3 - sourceMesh->getEdges()]);

		// Add 3 internal edges on the face of triangle
		//  e1_mid -> e2_mid
		targetMesh->getEdges()[addedEdgeCount].vertOrigin = currentExtendedFace->extendedEdge1->newEdgeS->vertDest;
		targetMesh->getEdges()[addedEdgeCount].vertDest = currentExtendedFace->extendedEdge2->newEdgeS->vertDest;
		currentExtendedFace->internalEdge12 = &(targetMesh->getEdges()[addedEdgeCount]);
		addedEdgeCount++;
		//  e2_mid -> e3_mid
		targetMesh->getEdges()[addedEdgeCount].vertOrigin = currentExtendedFace->extendedEdge2->newEdgeS->vertDest;
		targetMesh->getEdges()[addedEdgeCount].vertDest = currentExtendedFace->extendedEdge3->newEdgeS->vertDest;
		currentExtendedFace->internalEdge23 = &(targetMesh->getEdges()[addedEdgeCount]);
		addedEdgeCount++;
		//  e3_mid -> e1_mid
		targetMesh->getEdges()[addedEdgeCount].vertOrigin = currentExtendedFace->extendedEdge3->newEdgeS->vertDest;
		targetMesh->getEdges()[addedEdgeCount].vertDest = currentExtendedFace->extendedEdge1->newEdgeS->vertDest;
		currentExtendedFace->internalEdge31 = &(targetMesh->getEdges()[addedEdgeCount]);
		addedEdgeCount++;

		std::cout << "Edges added " << addedEdgeCount << std::endl;

	}
}

/**
 * Compute 3xFaceCount matrix identifying vertex triplets in each face of target mesh.
 * @param originalMesh Original mesh fed into the subdivision process
 * @param subdividedMesh Subdivided mesh we generate with updated vertices and edges
 * @param expectedFaceCount Count of faces expected in the final subdivided mesh
 * @param extendedFaces Array of extended faces linking each original face to 4 subdivided faces
 * @return 3 x expectedFaceCount array containing vertex indices of each face.
 */
std::vector<glm::ivec3> computeFaceIndices(const WEMesh* originalMesh, const WEMesh* subdividedMesh,
	int expectedFaceCount, const ExtendedFace* extendedFaces) {
	// Identify triangles in subdivided mesh, so we can generate face indices of a triangular mesh
	int addedFaceCount = 0;
	std::vector<glm::ivec3> faceIndices(expectedFaceCount);

	for (int f = 0; f < originalMesh->getFaceCount(); f++)
	{
		const ExtendedFace* currentExtendedFace = &(extendedFaces[f]);
		ExtendedEdge* party1[3] = { currentExtendedFace->extendedEdge1, currentExtendedFace->extendedEdge2,
								   currentExtendedFace->extendedEdge3 };
		ExtendedEdge* party2[3] = { currentExtendedFace->extendedEdge2, currentExtendedFace->extendedEdge3,
								   currentExtendedFace->extendedEdge1 };
		Edge* diagonalParty[3] = { currentExtendedFace->internalEdge12, currentExtendedFace->internalEdge23,
											  currentExtendedFace->internalEdge31 };
		//        Vertex* commonPoint = nullptr;
		Edge* party1Part[3];
		Edge* party2Part[3];

		for (int k = 0; k < 3; k++)
		{
			if (party1[k]->newEdgeS->vertOrigin == party2[k]->newEdgeS->vertOrigin)
			{
				party1Part[k] = party1[k]->newEdgeS;
				party2Part[k] = party2[k]->newEdgeS;
			}
			else if (party1[k]->newEdgeS->vertOrigin == party2[k]->newEdgeE->vertDest)
			{
				party1Part[k] = party1[k]->newEdgeS;
				party2Part[k] = party2[k]->newEdgeE;
			}
			else if (party1[k]->newEdgeE->vertDest == party2[k]->newEdgeE->vertDest)
			{
				party1Part[k] = party1[k]->newEdgeE;
				party2Part[k] = party2[k]->newEdgeE;
			}
			else if (party1[k]->newEdgeE->vertDest == party2[k]->newEdgeS->vertOrigin)
			{
				party1Part[k] = party1[k]->newEdgeE;
				party2Part[k] = party2[k]->newEdgeS;
			}
		}

		// Edges in CW order: party1: party2: diagonalParty
		for (int k = 0; k < 3; k++)
		{
			Vertex* c1 = getCommonVertex(party1Part[k], party2Part[k]);
			Vertex* c2 = getCommonVertex(party2Part[k], diagonalParty[k]);
			Vertex* c3 = getCommonVertex(diagonalParty[k], party1Part[k]);

			std::cout << "Face: " << c1 - subdividedMesh->getVertices() << ", " << c2 - subdividedMesh->getVertices()
				<< ", " << c3 - subdividedMesh->getVertices() << std::endl;

			/*faceIndices.col(addedFaceCount++) << c3 - subdividedMesh->getVertices(), c2 - subdividedMesh->getVertices(),
				c1 -
				subdividedMesh->getVertices();*/
			faceIndices[addedFaceCount].x = c1 - subdividedMesh->getVertices();
			faceIndices[addedFaceCount].y = c2 - subdividedMesh->getVertices();
			faceIndices[addedFaceCount].z = c3 - subdividedMesh->getVertices();
			addedFaceCount++;
		}
		Vertex* d1 = getCommonVertex(diagonalParty[0], diagonalParty[1]);
		Vertex* d2 = getCommonVertex(diagonalParty[1], diagonalParty[2]);
		Vertex* d3 = getCommonVertex(diagonalParty[2], diagonalParty[0]);

		/*faceIndices.col(addedFaceCount++) << d3 - subdividedMesh->getVertices(), d2 - subdividedMesh->getVertices(),
			d1 -
			subdividedMesh->getVertices();*/
		faceIndices[addedFaceCount].x = d1 - subdividedMesh->getVertices();
		faceIndices[addedFaceCount].y = d2 - subdividedMesh->getVertices();
		faceIndices[addedFaceCount].z = d3 - subdividedMesh->getVertices();

	}
	return faceIndices;
}

/**
 * Extract vertices from the subdivided mesh
 * @param subdividedMesh Subdivided mesh
 * @return
 */
std::vector<glm::vec3> computeVertexIndices(const WEMesh* subdividedMesh)
{
	// Identify vertices of triangular mesh
	std::vector<glm::vec3> positions(subdividedMesh->getVertexCount());
	for (int v = 0; v < subdividedMesh->getVertexCount(); v++)
	{
		positions[v] = subdividedMesh->getVertices()[v].getPosition();
	}
	return positions;
}

/**
 * Performs loop subdivision on sourceMesh and populate tMesh using the result. Source mesh is unchanged.
 * @param tMesh OBJ mesh filled with results of method
 * @param sourceMesh Source mesh to be tesselated. This mesh is unchanged.
 * @param applyEdgeRule if true, edge vertex position is calculated using Edge Geometric Rule of Loop Subdivision. If false, midpoint of edge is chosen.
 * @param applyVertexRule if true, Vertex Geometric rule is applied. Otherwise, vertices are simply copied forward.
 */
void LoopSubdivision::loopSubdivision(WEMesh* tMesh, const WEMesh* sourceMesh, bool applyVertexRule, bool applyEdgeRule)
{
	WEMesh _targetMesh;
	WEMesh* targetMesh = &_targetMesh;

	// Each edge gets partitioned by a vertex
	int newVertexCount = sourceMesh->getVertexCount() + sourceMesh->getEdgeCount();
	// Each edge gets divided into two and each face gets 3 more edges
	int newEdgeCount = sourceMesh->getEdgeCount() * 2 + sourceMesh->getFaceCount() * 3;
	//Each triangle face gets divided into 4
	int newFaceCount = sourceMesh->getFaceCount() * 4;

	ExtendedFace* extendedFaces = new ExtendedFace[sourceMesh->getFaceCount()];
	ExtendedEdge* extendedEdges = new ExtendedEdge[sourceMesh->getEdgeCount()];

	// Allocate resources to hold subdivided mesh
	targetMesh->allocateVertices(newVertexCount);
	targetMesh->allocateEdges(newEdgeCount);
	targetMesh->allocateFaces(newFaceCount);

	// Copy forward and update vertices using the vertex rule
	updateVertices(sourceMesh, applyVertexRule, targetMesh);

	int addedVertexCount = sourceMesh->getVertexCount();
	int addedEdgeCount = 0;

	// Subdivide edges
	subdivideEdges(sourceMesh, applyEdgeRule, targetMesh, extendedEdges, addedVertexCount, addedEdgeCount);
	assert(addedVertexCount == newVertexCount);

	std::cout << "Added vertices " << addedVertexCount << " Total should be " << newVertexCount << std::endl;


	// Subdivide faces
	subdivideFaces(sourceMesh, targetMesh, extendedFaces, extendedEdges, addedEdgeCount);
	assert(addedEdgeCount == newEdgeCount);

	std::cout << "Edges added " << addedEdgeCount << "/" << newEdgeCount << std::endl;


	// Create the subdivided OBJ mesh
	auto faceIndices = computeFaceIndices(sourceMesh, targetMesh, newFaceCount, extendedFaces);
	auto positions = computeVertexIndices(targetMesh);

	// Assign vertices and face indices to triangular mesh
	tMesh = new WEMesh();
	tMesh->loadModel(positions, faceIndices);

	delete[] extendedFaces;
	delete[] extendedEdges;
}
