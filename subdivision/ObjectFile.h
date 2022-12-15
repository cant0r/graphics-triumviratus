#pragma once

#include <vector>
#include <glm/glm.hpp>

class ObjectFile
{
	std::vector<glm::vec3> _triangleVertexes;
	std::vector<glm::vec3> _rawVertexes;
	std::vector<glm::ivec3> _rawFaces;

public:
	void addTriangleVertex(glm::vec3);
	std::vector<glm::vec3> getTriangleVertexes();
	void addRawVertex(glm::vec3);
	std::vector<glm::vec3> getRawVertexes();
	void addFace(glm::ivec3);
	std::vector<glm::ivec3> getFaces();
};

