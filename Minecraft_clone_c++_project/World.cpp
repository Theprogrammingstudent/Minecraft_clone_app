#include "World.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

World::World()
{
    // PASS 1 — generate all chunk block data first
    // every chunk must exist before we start meshing
    // so the mesher can check across chunk borders
    for (int x = -VIEW_DISTANCE; x <= VIEW_DISTANCE; x++)
    {
        for (int z = -VIEW_DISTANCE; z <= VIEW_DISTANCE; z++)
        {
            std::string key = makeKey(x, z);
            m_chunks[key].chunk = Chunk(x, z);
        }
    }

    // PASS 2 — build meshes now all chunks exist
    // passing 'this' lets the mesher call getChunk()
    // to check blocks in neighbouring chunks at borders
    for (int x = -VIEW_DISTANCE; x <= VIEW_DISTANCE; x++)
    {
        for (int z = -VIEW_DISTANCE; z <= VIEW_DISTANCE; z++)
        {
            std::string key = makeKey(x, z);

            m_chunks[key].mesher.buildMesh(m_chunks[key].chunk, this);

            m_chunks[key].renderer.uploadMesh(
                m_chunks[key].mesher.vertices,
                m_chunks[key].mesher.indices
            );
        }
    }
}

void World::draw(Shader& shader)
{
    for (auto& [key, data] : m_chunks)
    {
        // position each chunk in world space using its chunk coordinates
        float worldX = (float)(data.chunk.worldX * CHUNK_SIZE_X);
        float worldZ = (float)(data.chunk.worldZ * CHUNK_SIZE_Z);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(worldX, 0.0f, worldZ));

        shader.setMat4("uModel", model);
        data.renderer.draw((int)data.mesher.indices.size());
    }
}

const Chunk* World::getChunk(int chunkX, int chunkZ) const
{
    // look up chunk by coordinate key
    // returns nullptr if chunk doesn't exist
    // mesher treats nullptr as air — safe default
    std::string key = std::to_string(chunkX) + "," + std::to_string(chunkZ);
    auto it = m_chunks.find(key);
    if (it == m_chunks.end())
        return nullptr;

    return &it->second.chunk;
}

std::string World::makeKey(int chunkX, int chunkZ)
{
    return std::to_string(chunkX) + "," + std::to_string(chunkZ);
}