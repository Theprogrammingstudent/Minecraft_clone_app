#include "Chunk.h"
#include "FastNoiseLite.h"

// constructor now takes a world position in chunk coordinates
// worldX=1 means this chunk starts at x=16 in the world
// worldZ=2 means this chunk starts at z=32 in the world
// default values of 0,0 mean the chunk sits at the origin
Chunk::Chunk(int worldX, int worldZ)
    : worldX(worldX), worldZ(worldZ)  // store world position in member variables
{
    // create the noise generator
    FastNoiseLite noise;

    // FractalFBm layers multiple noise frequencies on top of each other
    // this gives you large hills with smaller bumps on top — more natural looking
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);

    // how many layers of noise to stack on top of each other
    // more octaves = more detail but slower to generate
    noise.SetFractalOctaves(4);

    // how much each layer contributes relative to the previous
    // 0.5 means each layer is half as strong as the one before
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);

    // scale — smaller number = zoomed out = bigger hills
    noise.SetFrequency(0.05f);

    // loop through every x,z column in the chunk
    for (int x = 0; x < CHUNK_SIZE_X; x++)
    {
        for (int z = 0; z < CHUNK_SIZE_Z; z++)
        {
            // convert local chunk coordinates to world coordinates for noise sampling
            // without this offset every chunk would sample x:0-15, z:0-15
            // and generate completely identical terrain
            // chunk (1,0) needs to sample x:16-31, chunk (2,0) needs x:32-47 etc
            float worldSampleX = (float)(x + worldX * CHUNK_SIZE_X);
            float worldSampleZ = (float)(z + worldZ * CHUNK_SIZE_Z);

            // GetNoise returns a value between -1 and 1
            // multiply by 8 to get a range of -8 to 8
            // add 10 to shift it up so terrain sits above y=0
            // result is a height between 2 and 18
            float noiseValue = noise.GetNoise(worldSampleX, worldSampleZ);
            int height = (int)(noiseValue * 8.0f) + 10;

            // fill blocks from the bottom up to the calculated height
            // the && y < CHUNK_SIZE_Y guard prevents writing outside the array
            // if noise produces a height taller than the chunk
            for (int y = 0; y < height && y < CHUNK_SIZE_Y; y++)
            {
                if (y == height - 1)
                    // top block is always grass — the surface layer
                    setBlock(x, y, z, BlockType::Grass);
                else if (y >= height - 3)
                    // next 3 blocks down are dirt — just below the surface
                    setBlock(x, y, z, BlockType::Dirt);
                else
                    // everything below is stone — the deep layer
                    setBlock(x, y, z, BlockType::Stone);
            }
        }
    }
}

int Chunk::getIndex(int x, int y, int z) const
{
    // converts 3D position to a flat 1D array index
    // think of it as: x = position along a corridor
    //                 z = which corridor you're in
    //                 y = which floor of the building
    // stored flat for cache efficiency — sequential memory access is faster
    return x + (y * CHUNK_SIZE_X) + (z * CHUNK_SIZE_X * CHUNK_SIZE_Y);
}

bool Chunk::inBounds(int x, int y, int z) const
{
    // returns true only if the position is inside this chunk's boundaries
    // used by getBlock and setBlock to avoid reading/writing outside the array
    // also used by the mesher when checking neighbouring blocks at chunk edges
    return x >= 0 && x < CHUNK_SIZE_X &&
        y >= 0 && y < CHUNK_SIZE_Y &&
        z >= 0 && z < CHUNK_SIZE_Z;
}

Block Chunk::getBlock(int x, int y, int z) const
{
    // safety check — return air if position is out of bounds
    // the mesher calls this for every neighbour of every block
    // out of bounds neighbours are treated as air so edge faces get drawn
    if (!inBounds(x, y, z))
        return Block();  // default Block constructor sets type to Air

    return blocks[getIndex(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, BlockType type)
{
    // safety check — silently ignore writes outside the chunk boundary
    // prevents array out of bounds crashes during world generation
    if (!inBounds(x, y, z))
        return;

    blocks[getIndex(x, y, z)].type = type;
}