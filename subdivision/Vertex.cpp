#include "Vertex.h"

void Vertex::setEdge(Edge* nEdge) {
	mEdge = nEdge;
}

Edge* Vertex::getEdge() {
	return mEdge;
}

Vertex::Vertex() {
	mEdge = nullptr;
	mPosition = glm::vec3(0, 0, 0);
}

float Vertex::getX() {
	return mPosition.x;
}

float Vertex::getY() {
	return mPosition.y;
}

float Vertex::getZ() {
	return mPosition.z;
}

void Vertex::setPosition(float x, float y, float z) {
	mPosition = glm::vec3(x, y, z);
}

void Vertex::setX(float x) {
	mPosition[0] = x;
}

void Vertex::setY(float y) {
	mPosition[1] = y;
}

void Vertex::setZ(float z) {
	mPosition[2] = z;
}

Vertex::Vertex(float x, float y, float z) : Vertex() {
	setPosition(x, y, z);
}

glm::vec3 Vertex::getPosition() {
	return mPosition;
}

void Vertex::setPosition(glm::vec3 pos) {
	this->mPosition = pos;
}