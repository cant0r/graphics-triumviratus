#include "ObjectFile.h"

void ObjectFile::addTriangleVertex(glm::vec3 vertex)
{
	this->_triangleVertexes.push_back(vertex);
}

std::vector<glm::vec3> ObjectFile::getTriangleVertexes()
{
	return this->_triangleVertexes;
}

void ObjectFile::addRawVertex(glm::vec3 vertex)
{
	this->_rawVertexes.push_back(vertex);
}

std::vector<glm::vec3> ObjectFile::getRawVertexes()
{
	return this->_rawVertexes;
}

void ObjectFile::addFace(glm::vec3 face)
{
	this->_rawFaces.push_back(face);
}

std::vector<glm::vec3> ObjectFile::getFaces()
{
	return this->_rawFaces;
}