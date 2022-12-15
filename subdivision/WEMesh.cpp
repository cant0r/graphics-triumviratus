#include "WEMesh.h"
#include <iostream>

WEMesh::WEMesh() 
{
	vertices = nullptr;
	faces = nullptr;
	edges = nullptr;
	vertexCount = 0;
	faceCount = 0;
	edgeCount = 0;
	modelLoaded = false;
}

void WEMesh::unloadModel() 
{
	if (!modelLoaded)
		return;

	vertexCount = 0;
	faceCount = 0;
	edgeCount = 0;

	delete[] vertices;
	delete[] edges;
	delete[] faces;

	vertices = nullptr;
	edges = nullptr;
	faces = nullptr;

	modelLoaded = false;
}

/**
 * Populates WingedEdge faces, edges and vertices with appropriate inter-links using provided vertices and faces (vertex indices)
 * @param vertices 3xn array of vertices
 * @param faces 3xt array of vertex indices of each triangular face
 * @return true if success
 */
bool WEMesh::loadModel(std::vector<glm::vec3>& vertices, std::vector<glm::ivec3>& faces)
{
	std::cout << std::endl << "Populating Winged Edge" << std::endl;
	unloadModel();

	vertexCount = vertices.size();
	faceCount = faces.size();
	edgeCount = faceCount * 2 + 1;

	this->vertices = new Vertex[vertexCount];
	this->edges = new Edge[edgeCount];
	this->faces = new Face[faceCount];

	// Used to locate an edge using a vertex pair. Think of this as a hash map
	Edge*** vertexPairEdgeArray = new Edge**[vertexCount];
	for (int i = 0; i < vertexCount; i++) 
	{
		vertexPairEdgeArray[i] = new Edge*[vertexCount];
		for (int j = 0; j < vertexCount; j++) 
		{
			vertexPairEdgeArray[i][j] = nullptr;
		}
	}

	// load vertices
	for (int i = 0; i < vertexCount; i++) 
	{
		this->vertices[i].setPosition(vertices[i].x, vertices[i].y, vertices[i].z);
	}

	int edgeCountIterator = 0;
	// Loop through faces and populate edges. Fill LeftFace and RightFace.
	for (int f = 0; f < faceCount; f++)
	{
		Edge* edgeBank[] = { nullptr, nullptr, nullptr }; //keep track of 3 edges belonging to this face
		for (int k = 0; k < 3; k++)
		{
			int v1_index = faces[f][k];
			int v2_index = faces[f][(k + 1) % 3];
			std::cout << "v1_index = " << v1_index << std::endl;
			std::cout << "v2_index = " << v2_index << std::endl;
			// v1 => v2 is counter clockwise direction
			// v2 => v1 is clockwise direction
			// v2 => v1 is my edge in clockwise direction. But need to check whether v1 => v2 is already acquired by another face in clockwise direction
			if (vertexPairEdgeArray[v1_index][v2_index] == nullptr)
			{
				// OK. the edge v1 => v2 does not exist. I can create edge v2 => v1.
				vertexPairEdgeArray[v2_index][v1_index] = &(this->edges[edgeCountIterator]);
				this->edges[edgeCountIterator].vertOrigin = &this->vertices[v2_index];
				this->edges[edgeCountIterator].vertDest = &this->vertices[v1_index];
				// I am the right face.
				this->edges[edgeCountIterator].rightFace = &this->faces[f];
				if (k == 0)
				{
					this->faces[f].setEdge(&(this->edges[edgeCountIterator]));
				}
				edgeBank[k] = &(this->edges[edgeCountIterator]);

				// Check whether two end nodes has an edge. If not, set this edge
				if (this->vertices[v1_index].getEdge() == nullptr)
					this->vertices[v1_index].setEdge(&(this->edges[edgeCountIterator]));

				if (this->vertices[v2_index].getEdge() == nullptr)
					this->vertices[v2_index].setEdge(&(this->edges[edgeCountIterator]));

				edgeCountIterator++;
			}
			else
			{
				// Edge v1 => v2 already exists. I cant create v2=>v1. Instead, I should be the other face
				vertexPairEdgeArray[v1_index][v2_index]->leftFace = &this->faces[f];
				if (k == 0)
				{
					this->faces[f].setEdge(vertexPairEdgeArray[v1_index][v2_index]);
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
				vertexPairEdgeArray[v2_index][v1_index]->edgeRightCW = edgeBank[c];
				vertexPairEdgeArray[v2_index][v1_index]->edgeRightCCW = edgeBank[(k + 1) % 3];
			}
			else if (vertexPairEdgeArray[v1_index][v2_index] != nullptr) 
			{
				assert(vertexPairEdgeArray[v2_index][v1_index] == nullptr);
				// I have joined to v1 => v2 face already in existence. So, I am the left face. k is anticlockwise.
				vertexPairEdgeArray[v1_index][v2_index]->edgeLeftCCW = edgeBank[(k + 1) % 3];

				int c = k - 1;
				if (c < 0) c = 2;
				vertexPairEdgeArray[v1_index][v2_index]->edgeLeftCW = edgeBank[c];
			}
		}


	}
	std::cout << std::endl << "Winged Edge Populated" << std::endl;
	std::cout << "Winged Edge: Actual Edge Count: " << edgeCountIterator << std::endl;
	std::cout << "Winged Edge: Expected Edge Count (Face Count * 2 + 1): " << faceCount * 2 + 1 << std::endl;

	for (int i = 0; i < vertexCount; i++)
	{
		delete[] vertexPairEdgeArray[i];
	}
	delete[] vertexPairEdgeArray;

	modelLoaded = true;
	return true;
}

WEMesh::~WEMesh() 
{
	unloadModel();
}

Vertex* WEMesh::getVertices() const 
{
	return vertices;
}

Edge* WEMesh::getEdges() const 
{
	return edges;
}

Face* WEMesh::getFaces() const 
{
	return faces;
}

int WEMesh::getVertexCount() const 
{
	return vertexCount;
}

int WEMesh::getEdgeCount() const 
{
	return edgeCount;
}

int WEMesh::getFaceCount() const 
{
	return faceCount;
}

void WEMesh::allocateVertices(int v) 
{
	delete[] vertices;
	vertexCount = v;
	vertices = new Vertex[vertexCount];
}
void WEMesh::allocateEdges(int e) 
{
	delete[] edges;
	edgeCount = e;
	edges = new Edge[edgeCount];
}
void WEMesh::allocateFaces(int f) 
{
	delete[] faces;
	faceCount = f;
	faces = new Face[faceCount];
}

bool WEMesh::isModelLoaded() const 
{
	return modelLoaded;
}