#pragma once
#include <vector>
#include "Chunk.h"

// a single vertex — position in 3D space
// we'll add texture coordinates and normals later
struct Vertex
{
    float x, y, z;  // position
};

// the mesher takes a chunk and builds a list of vertices and indices
// that can be uploaded to the GPU and drawn
class ChunkMesher
{
public:
    // these are filled by buildMesh() and read by the renderer
    std::vector<Vertex>       vertices;  // list of unique vertex positions
    std::vector<unsigned int> indices;   // list of indices into the vertex list

    // takes a chunk, loops through every block, checks each face
    // if the face is exposed to air it gets added to the mesh
    void buildMesh(const Chunk& chunk);

private:
    // adds one quad (two triangles = one block face) to the mesh
    // takes four corner positions and adds them as two triangles
    void addFace(
        float x0, float y0, float z0,   // corner 0
        float x1, float y1, float z1,   // corner 1
        float x2, float y2, float z2,   // corner 2
        float x3, float y3, float z3    // corner 3
    );
};