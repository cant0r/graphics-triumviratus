#include "Vertex.h"
#include "Face.h"

void Vertex::setEdge(Edge* nEdge) {
    edge = nEdge;
}

Edge* Vertex::getEdge() {
    return edge;
}

Vertex::Vertex() {
    edge = nullptr;
    position = glm::vec3(0, 0, 0);
}

float Vertex::getX() {
    return position.x;
}

float Vertex::getY() {
    return position.y;
}

float Vertex::getZ() {
    return position.z;
}

void Vertex::setPosition(float x, float y, float z) {
    position = glm::vec3(x, y, z);
}

void Vertex::setX(float x) {
    position.x = x;
}

void Vertex::setY(float y) {
    position.y = y;
}

void Vertex::setZ(float z) {
    position.z = z;
}

Vertex::Vertex(float x, float y, float z) : Vertex() {
    setPosition(x, y, z);
}

glm::vec3 Vertex::getPosition() {
    return position;
}

void Vertex::setPosition(glm::vec3 pos) {
    this->position = pos;
}