#include "Chunk.h"

Chunk::Chunk()
{
    // fill the chunk with a simple test pattern
    // everything is air by default from Block's constructor
    // we'll add proper world generation in Phase 3
    for (int x = 0; x < CHUNK_SIZE_X; x++)
    {
        for (int z = 0; z < CHUNK_SIZE_Z; z++)
        {
            // set the bottom few layers to give us something to look at
            // y=0 is the bottom of the chunk
            setBlock(x, 0, z, BlockType::Stone);  // bottom layer stone
            setBlock(x, 1, z, BlockType::Dirt);   // middle layer dirt
            setBlock(x, 2, z, BlockType::Grass);  // top layer grass
        }
    }
}

int Chunk::getIndex(int x, int y, int z) const
{
    // converts 3D position to flat array index
    // think of it as: x selects the column,
    // y selects the floor, z selects the row
    return x + (y * CHUNK_SIZE_X) + (z * CHUNK_SIZE_X * CHUNK_SIZE_Y);
}

bool Chunk::inBounds(int x, int y, int z) const
{
    // returns true if the position is inside the chunk
    // used by the mesher to avoid reading outside the array
    return x >= 0 && x < CHUNK_SIZE_X &&
        y >= 0 && y < CHUNK_SIZE_Y &&
        z >= 0 && z < CHUNK_SIZE_Z;
}

Block Chunk::getBlock(int x, int y, int z) const
{
    // safety check — return air if out of bounds
    // prevents array out of bounds crashes at chunk edges
    if (!inBounds(x, y, z))
        return Block();  // default Block is Air

    return blocks[getIndex(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, BlockType type)
{
    // safety check — silently ignore out of bounds writes
    if (!inBounds(x, y, z))
        return;

    blocks[getIndex(x, y, z)].type = type;
}