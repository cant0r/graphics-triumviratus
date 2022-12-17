#pragma once
#include "WEMesh.h"
class LoopSubdivision
{
    void loopSubdivision(WEMesh* tMesh, const WEMesh* sourceMesh, bool applyVertexRule,
        bool applyEdgeRule);
};

