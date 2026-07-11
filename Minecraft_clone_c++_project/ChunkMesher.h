#pragma once
#include <vector>
#include "Chunk.h"

// forward declare World to avoid circular includes
// World.h includes ChunkMesher.h so we can't include World.h here
class World;

// each vertex carries position AND texture coordinate
// u,v tell the GPU which part of the texture atlas to sample
// u = horizontal (0.0 left → 1.0 right)
// v = vertical   (0.0 bottom → 1.0 top)
struct Vertex
{
    float x, y, z;   // position in 3D space
    float u, v;       // texture coordinate in the atlas
};

class ChunkMesher
{
public:
    // filled by buildMesh() and read by the renderer
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    // world pointer added so mesher can check blocks in neighbouring chunks
    // fixes seams at chunk borders — defaults to nullptr for safety
    void buildMesh(const Chunk& chunk, const World* world = nullptr);

private:
    // now takes uv coordinates and block type per face
    void addFace(
        float x0, float y0, float z0, float u0, float v0,
        float x1, float y1, float z1, float u1, float v1,
        float x2, float y2, float z2, float u2, float v2,
        float x3, float y3, float z3, float u3, float v3,
        BlockType type
    );
};