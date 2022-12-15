#pragma once
class Vertex;
class Face;

class Edge 
{
public:
    Edge();

    Edge(Vertex* origin, Vertex* destination);

    Vertex* vertOrigin; // Origin Vertex
    Vertex* vertDest; // Destination Vertex
    Face* leftFace; // Left Face
    Face* rightFace; // Right Face
    Edge* edgeLeftCW; // Left Clockwise Edge
    Edge* edgeLeftCCW; // Left CounterClockwise Edge
    Edge* edgeRightCW; // Right Clockwise Edge
    Edge* edgeRightCCW; // Right Counterclockwise edge
};

