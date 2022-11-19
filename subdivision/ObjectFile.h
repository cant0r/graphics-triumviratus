#pragma once

#include <vector>
#include <glm/glm.hpp>

class ObjectFile
{
	std::vector<glm::vec3> _triangleVertexes;
	std::vector<glm::ivec3> _faceVertexes;

public:
	void addTriangleVertex(glm::vec3);
	std::vector<glm::vec3> getTriangleVertexes();
	void addFaceVertex(glm::ivec3);
	std::vector<glm::ivec3> getFaceVertexes();
};

