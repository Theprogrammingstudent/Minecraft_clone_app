#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "Chunk.h"
#include "ChunkMesher.h"
#include "Renderer.h"

// a ChunkData struct bundles everything one chunk needs
// each chunk has its own block data, mesh, and renderer
// the renderer owns a VAO/VBO/EBO on the GPU for that chunk alone
struct ChunkData
{
    Chunk chunk;          // block data — 16x16x16 grid
    ChunkMesher mesher;   // builds the visible face mesh
    Renderer renderer;    // uploads and draws that mesh on the GPU
};

class World
{
public:
    // how many chunks to generate in each direction from the origin
    // 2 means a 5x5 grid (2 left, centre, 2 right)
    static constexpr int VIEW_DISTANCE = 2;

    // generates all chunks in the view distance and builds their meshes
    World();

    // draws all chunks using the provided shader
    // each chunk needs its own model matrix so it sits in the right world position
    void draw(class Shader& shader);

private:
    // stores all chunk data keyed by a string like "0,0" or "1,-2"
    // unordered_map gives O(1) lookup by chunk coordinate
    std::unordered_map<std::string, ChunkData> m_chunks;

    // converts chunk coordinates to a string key for the map
    // e.g. chunkX=1, chunkZ=2 becomes "1,2"
    std::string makeKey(int chunkX, int chunkZ);
};