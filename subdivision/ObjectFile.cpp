#include "ObjectFile.h"

void ObjectFile::addTriangleVertex(glm::vec3 vertex)
{
	this->_triangleVertexes.push_back(vertex);
}

std::vector<glm::vec3> ObjectFile::getTriangleVertexes()
{
	return this->_triangleVertexes;
}

void ObjectFile::addFaceVertex(glm::ivec3 vertex)
{
	this->_faceVertexes.push_back(vertex);
}

std::vector<glm::ivec3> ObjectFile::getFaceVertexes()
{
	return this->_faceVertexes;
}