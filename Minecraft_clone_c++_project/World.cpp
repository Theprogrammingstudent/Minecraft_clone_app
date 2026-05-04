#include "World.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

World::World()
{
    // generate a VIEW_DISTANCE x VIEW_DISTANCE grid of chunks
    // VIEW_DISTANCE of 2 gives chunks at -2,-1,0,1,2 in both x and z
    // that's a 5x5 grid = 25 chunks total
    for (int x = -VIEW_DISTANCE; x <= VIEW_DISTANCE; x++)
    {
        for (int z = -VIEW_DISTANCE; z <= VIEW_DISTANCE; z++)
        {
            std::string key = makeKey(x, z);

            // create the chunk at this world position
            // the chunk constructor uses x,z to offset its noise sampling
            // so each chunk generates unique terrain that joins seamlessly
            m_chunks[key].chunk = Chunk(x, z);

            // build the mesh — loops every block, finds visible faces
            m_chunks[key].mesher.buildMesh(m_chunks[key].chunk);

            // upload the mesh to the GPU
            // each ChunkData has its own Renderer with its own VAO/VBO/EBO
            m_chunks[key].renderer.uploadMesh(
                m_chunks[key].mesher.vertices,
                m_chunks[key].mesher.indices
            );
        }
    }
}

void World::draw(Shader& shader)
{
    // loop every chunk and draw it at its correct world position
    for (auto& [key, data] : m_chunks)
    {
        // calculate where this chunk sits in world space
        // each chunk is CHUNK_SIZE_X wide and CHUNK_SIZE_Z deep
        // so chunk (1,0) starts at world x=16, chunk (2,0) starts at x=32
        float worldX = (float)(data.chunk.worldX * CHUNK_SIZE_X);
        float worldZ = (float)(data.chunk.worldZ * CHUNK_SIZE_Z);

        // build the model matrix for this chunk
        // translates from the origin to where this chunk lives in the world
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(worldX, 0.0f, worldZ));

        // send this chunk's model matrix to the shader
        // each chunk gets a different matrix so they sit in different positions
        shader.setMat4("uModel", model);

        data.renderer.draw((int)data.mesher.indices.size());
    }
}

std::string World::makeKey(int chunkX, int chunkZ)
{
    // creates a unique string key from chunk coordinates
    // used to store and look up chunks in the unordered_map
    return std::to_string(chunkX) + "," + std::to_string(chunkZ);
}