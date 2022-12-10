#pragma once

#include <vector>
#include <glm/glm.hpp>

class ObjectFile
{
	std::vector<glm::vec3> _triangleVertexes;

public:
	void addTriangleVertex(glm::vec3);
	std::vector<glm::vec3> getTriangleVertexes();
};

