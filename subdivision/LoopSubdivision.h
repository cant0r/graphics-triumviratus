#pragma once
#include "WEMesh.h"
class LoopSubdivision
{
public:
    void loopSubdivision(WEMesh* tMesh, const WEMesh* sourceMesh, bool applyVertexRule,
        bool applyEdgeRule);
};

