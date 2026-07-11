#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "Chunk.h"
#include "ChunkMesher.h"
#include "Renderer.h"

struct ChunkData
{
    Chunk chunk;
    ChunkMesher mesher;
    Renderer renderer;
};

class World
{
public:
    static constexpr int VIEW_DISTANCE = 2;

    World();

    void draw(class Shader& shader);

    // returns a pointer to the chunk at chunk coordinates
    // returns nullptr if the chunk doesn't exist
    // used by ChunkMesher to check blocks across chunk borders
    const Chunk* getChunk(int chunkX, int chunkZ) const;

private:
    std::unordered_map<std::string, ChunkData> m_chunks;
    std::string makeKey(int chunkX, int chunkZ);
};