#pragma once

#include <glm/ext.hpp>

class Edge;

class Vertex
{
	glm::vec3 position;
	Edge* edge;

public:
    Vertex(float x, float y, float z);
    Vertex();

    float getX();
    float getY();
    float getZ();

    void setX(float x);
    void setY(float y);
    void setZ(float z);

    glm::vec3 getPosition();

    void setPosition(float x, float y, float z);
    void setPosition(glm::vec3 pos);

    Edge* getEdge();

    void setEdge(Edge* nEdge);
};

