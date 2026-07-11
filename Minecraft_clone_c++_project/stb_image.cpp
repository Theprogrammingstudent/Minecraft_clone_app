// this define must exist in exactly one .cpp file
// it tells stb_image to include the actual implementation
// without this you get linker errors — the functions exist
// in the header but nothing defines their body
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"