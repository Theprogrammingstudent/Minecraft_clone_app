#include "ChunkMesher.h"
#include "World.h"

struct UVRect
{
    float uMin, uMax;
    float vMin, vMax;
};

static UVRect getUV(BlockType type)
{
    float slotWidth = 1.0f / 3.0f;
    switch (type)
    {
    case BlockType::Grass: return { 0.0f * slotWidth, 1.0f * slotWidth, 0.0f, 1.0f };
    case BlockType::Dirt:  return { 1.0f * slotWidth, 2.0f * slotWidth, 0.0f, 1.0f };
    case BlockType::Stone: return { 2.0f * slotWidth, 3.0f * slotWidth, 0.0f, 1.0f };
    default:               return { 0.0f, slotWidth, 0.0f, 1.0f };
    }
}

static Block getBlockWorld(const Chunk& chunk, const World* world, int x, int y, int z)
{
    // fast path — position is inside this chunk
    if (chunk.inBounds(x, y, z))
        return chunk.getBlock(x, y, z);

    // out of bounds vertically — treat as air
    if (y < 0 || y >= CHUNK_SIZE_Y)
        return Block();

    // work out which neighbouring chunk this position belongs to
    int neighbourChunkX = chunk.worldX;
    int neighbourChunkZ = chunk.worldZ;
    int localX = x;
    int localZ = z;

    // crossed the x boundary
    if (x < 0)
    {
        neighbourChunkX -= 1;
        localX = x + CHUNK_SIZE_X;
    }
    else if (x >= CHUNK_SIZE_X)
    {
        neighbourChunkX += 1;
        localX = x - CHUNK_SIZE_X;
    }

    // crossed the z boundary
    if (z < 0)
    {
        neighbourChunkZ -= 1;
        localZ = z + CHUNK_SIZE_Z;
    }
    else if (z >= CHUNK_SIZE_Z)
    {
        neighbourChunkZ += 1;
        localZ = z - CHUNK_SIZE_Z;
    }

    // ask the world for the neighbouring chunk
    if (!world)
        return Block();

    const Chunk* neighbour = world->getChunk(neighbourChunkX, neighbourChunkZ);
    if (!neighbour)
        return Block();

    return neighbour->getBlock(localX, y, localZ);
}

void ChunkMesher::buildMesh(const Chunk& chunk, const World* world)
{
    vertices.clear();
    indices.clear();

    for (int x = 0; x < CHUNK_SIZE_X; x++)
    {
        for (int y = 0; y < CHUNK_SIZE_Y; y++)
        {
            for (int z = 0; z < CHUNK_SIZE_Z; z++)
            {
                Block block = chunk.getBlock(x, y, z);

                if (block.isAir())
                    continue;

                float fx = (float)x;
                float fy = (float)y;
                float fz = (float)z;

                UVRect uv = getUV(block.type);

                // TOP face
                if (getBlockWorld(chunk, world, x, y + 1, z).isAir())
                    addFace(
                        fx, fy + 1, fz, uv.uMin, uv.vMax,
                        fx + 1, fy + 1, fz, uv.uMax, uv.vMax,
                        fx + 1, fy + 1, fz + 1, uv.uMax, uv.vMin,
                        fx, fy + 1, fz + 1, uv.uMin, uv.vMin,
                        block.type
                    );

                // BOTTOM face
                if (getBlockWorld(chunk, world, x, y - 1, z).isAir())
                    addFace(
                        fx, fy, fz + 1, uv.uMin, uv.vMin,
                        fx + 1, fy, fz + 1, uv.uMax, uv.vMin,
                        fx + 1, fy, fz, uv.uMax, uv.vMax,
                        fx, fy, fz, uv.uMin, uv.vMax,
                        block.type
                    );

                // FRONT face
                if (getBlockWorld(chunk, world, x, y, z + 1).isAir())
                    addFace(
                        fx, fy, fz + 1, uv.uMin, uv.vMin,
                        fx + 1, fy, fz + 1, uv.uMax, uv.vMin,
                        fx + 1, fy + 1, fz + 1, uv.uMax, uv.vMax,
                        fx, fy + 1, fz + 1, uv.uMin, uv.vMax,
                        block.type
                    );

                // BACK face
                if (getBlockWorld(chunk, world, x, y, z - 1).isAir())
                    addFace(
                        fx + 1, fy, fz, uv.uMin, uv.vMin,
                        fx, fy, fz, uv.uMax, uv.vMin,
                        fx, fy + 1, fz, uv.uMax, uv.vMax,
                        fx + 1, fy + 1, fz, uv.uMin, uv.vMax,
                        block.type
                    );

                // RIGHT face
                if (getBlockWorld(chunk, world, x + 1, y, z).isAir())
                    addFace(
                        fx + 1, fy, fz + 1, uv.uMin, uv.vMin,
                        fx + 1, fy, fz, uv.uMax, uv.vMin,
                        fx + 1, fy + 1, fz, uv.uMax, uv.vMax,
                        fx + 1, fy + 1, fz + 1, uv.uMin, uv.vMax,
                        block.type
                    );

                // LEFT face
                if (getBlockWorld(chunk, world, x - 1, y, z).isAir())
                    addFace(
                        fx, fy, fz, uv.uMin, uv.vMin,
                        fx, fy, fz + 1, uv.uMax, uv.vMin,
                        fx, fy + 1, fz + 1, uv.uMax, uv.vMax,
                        fx, fy + 1, fz, uv.uMin, uv.vMax,
                        block.type
                    );
            }
        }
    }
}

void ChunkMesher::addFace(
    float x0, float y0, float z0, float u0, float v0,
    float x1, float y1, float z1, float u1, float v1,
    float x2, float y2, float z2, float u2, float v2,
    float x3, float y3, float z3, float u3, float v3,
    BlockType type)
{
    unsigned int base = (unsigned int)vertices.size();

    vertices.push_back({ x0, y0, z0, u0, v0 });
    vertices.push_back({ x1, y1, z1, u1, v1 });
    vertices.push_back({ x2, y2, z2, u2, v2 });
    vertices.push_back({ x3, y3, z3, u3, v3 });

    indices.push_back(base + 0);
    indices.push_back(base + 1);
    indices.push_back(base + 2);
    indices.push_back(base + 0);
    indices.push_back(base + 2);
    indices.push_back(base + 3);
}