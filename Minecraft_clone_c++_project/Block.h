#pragma once
#include <cstdint>  // defines uint8_t, uint16_t, uint32_t etc

// BlockType defines every possible block in the game
// uint8_t means it's stored as a single byte (0-255)
// that's enough for 256 block types and keeps memory usage tiny
// a chunk of 4096 blocks at 1 byte each = 4kb — very cache friendly
enum class BlockType : uint8_t
{
    Air = 0,  // empty space — never rendered
    Grass = 1,  // top layer
    Dirt = 2,  // below grass
    Stone = 3,  // deep underground
};

// Block is the smallest unit in the world
// just a type for now — later you could add
// metadata, light level, moisture etc
struct Block
{
    // default to air so unset blocks are empty
    BlockType type = BlockType::Air;

    // helper — returns true if this block is air
    // used by the mesher to decide if a face is visible
    bool isAir() const
    {
        return type == BlockType::Air;
    }
};