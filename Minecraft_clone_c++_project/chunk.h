#pragma once
#include <cstdint>
#include "Block.h"

// chunk dimensions — 16x16x16 is the Minecraft standard
// defined as constants so you can change them in one place
constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 16;
constexpr int CHUNK_SIZE_Z = 16;

class Chunk
{
public:
    // 3D array of blocks stored flat in a 1D array
    // flat storage is more cache friendly than a 3D array
    // we convert 3D coordinates to a 1D index with getIndex()
    Block blocks[CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z];

    Chunk();

    // get a block at a local chunk position
    Block getBlock(int x, int y, int z) const;

    // set a block at a local chunk position
    void setBlock(int x, int y, int z, BlockType type);

    // check if a position is inside the chunk boundaries
    bool inBounds(int x, int y, int z) const;

private:
    // converts 3D coordinates into a flat 1D array index
    // this is the key formula for flat 3D array storage
    int getIndex(int x, int y, int z) const;
};