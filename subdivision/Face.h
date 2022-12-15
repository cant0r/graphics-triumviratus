#pragma once
class Edge;

class Face 
{
private:
    Edge* edge; //First edge of face
public:
    Face();
    Edge* getEdge() const;
    void setEdge(Edge* nEdge);
};

