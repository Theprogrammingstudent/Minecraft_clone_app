#include "ChunkMesher.h"

void ChunkMesher::buildMesh(const Chunk& chunk)
{
    // clear any previous mesh data
    vertices.clear();
    indices.clear();

    // loop through every block position in the chunk
    for (int x = 0; x < CHUNK_SIZE_X; x++)
    {
        for (int y = 0; y < CHUNK_SIZE_Y; y++)
        {
            for (int z = 0; z < CHUNK_SIZE_Z; z++)
            {
                // get the block at this position
                Block block = chunk.getBlock(x, y, z);

                // air blocks have no faces — skip them
                if (block.isAir())
                    continue;

                // convert int position to float for vertex data
                float fx = (float)x;
                float fy = (float)y;
                float fz = (float)z;

                // check each of the 6 faces
                // only add a face if the neighbour in that direction is air
                // this is the core of the face culling algorithm

                // TOP face — neighbour is one block above (y+1)
                if (chunk.getBlock(x, y + 1, z).isAir())
                    addFace(
                        fx, fy + 1, fz,        // back left
                        fx + 1, fy + 1, fz,        // back right
                        fx + 1, fy + 1, fz + 1,    // front right
                        fx, fy + 1, fz + 1     // front left
                    );

                // BOTTOM face — neighbour is one block below (y-1)
                if (chunk.getBlock(x, y - 1, z).isAir())
                    addFace(
                        fx, fy, fz + 1,        // front left
                        fx + 1, fy, fz + 1,        // front right
                        fx + 1, fy, fz,            // back right
                        fx, fy, fz             // back left
                    );

                // FRONT face — neighbour is one block forward (z+1)
                if (chunk.getBlock(x, y, z + 1).isAir())
                    addFace(
                        fx, fy, fz + 1,    // bottom left
                        fx + 1, fy, fz + 1,    // bottom right
                        fx + 1, fy + 1, fz + 1,    // top right
                        fx, fy + 1, fz + 1     // top left
                    );

                // BACK face — neighbour is one block behind (z-1)
                if (chunk.getBlock(x, y, z - 1).isAir())
                    addFace(
                        fx + 1, fy, fz,        // bottom right
                        fx, fy, fz,        // bottom left
                        fx, fy + 1, fz,        // top left
                        fx + 1, fy + 1, fz         // top right
                    );

                // RIGHT face — neighbour is one block right (x+1)
                if (chunk.getBlock(x + 1, y, z).isAir())
                    addFace(
                        fx + 1, fy, fz + 1,    // front bottom
                        fx + 1, fy, fz,        // back bottom
                        fx + 1, fy + 1, fz,        // back top
                        fx + 1, fy + 1, fz + 1     // front top
                    );

                // LEFT face — neighbour is one block left (x-1)
                if (chunk.getBlock(x - 1, y, z).isAir())
                    addFace(
                        fx, fy, fz,        // back bottom
                        fx, fy, fz + 1,    // front bottom
                        fx, fy + 1, fz + 1,    // front top
                        fx, fy + 1, fz         // back top
                    );
            }
        }
    }
}

void ChunkMesher::addFace(
    float x0, float y0, float z0,
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3)
{
    // record where this face's vertices start in the vertex list
    // we need this to write correct indices
    unsigned int base = (unsigned int)vertices.size();

    // add the four corners of this face as vertices
    vertices.push_back({ x0, y0, z0 });  // corner 0
    vertices.push_back({ x1, y1, z1 });  // corner 1
    vertices.push_back({ x2, y2, z2 });  // corner 2
    vertices.push_back({ x3, y3, z3 });  // corner 3

    // add two triangles using indices into the vertex list
    // 0-1-2 is the first triangle, 0-2-3 is the second
    // together they form one quad (one block face)
    indices.push_back(base + 0);
    indices.push_back(base + 1);
    indices.push_back(base + 2);
    indices.push_back(base + 0);
    indices.push_back(base + 2);
    indices.push_back(base + 3);
}