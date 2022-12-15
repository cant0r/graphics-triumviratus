#include "Face.h"

Edge* Face::getEdge() const {
    return edge;
}

void Face::setEdge(Edge* nEdge) {
    edge = nEdge;
}

Face::Face() {
    edge = nullptr;
}