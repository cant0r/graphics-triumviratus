#pragma once

#include "WingedEdge.h"

namespace Subdivision {
    /**
     * Performs loop subdivision on sourceMesh and populate tMesh using the result. Source mesh is unchanged.
     * @param tMesh OBJ mesh filled with results of method
     * @param sourceMesh Source mesh to be tesselated. This mesh is unchanged.
     * @param applyEdgeRule if true, edge vertex position is calculated using Edge Geometric Rule of Loop Subdivision. If false, midpoint of edge is chosen.
     * @param applyVertexRule if true, Vertex Geometric rule is applied. Otherwise, vertices are simply copied forward.
     */
    void loopSubdivision(WingedEdge* tMesh, const WingedEdge* sourceMesh, bool applyVertexRule, bool applyEdgeRule);
}