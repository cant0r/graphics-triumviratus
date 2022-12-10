#include "Face.h"

Edge* Face::getEdge() const {
    return mEdge;
}

void Face::setEdge(Edge* nEdge) {
    mEdge = nEdge;
}

Face::Face() {
    mEdge = nullptr;
}