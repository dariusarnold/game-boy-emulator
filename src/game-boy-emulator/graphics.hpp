#pragma once

#include "SDL_opengl.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <span>


namespace graphics {
/**
 * Functions in this namespace are for dealing with external graphic libraries.
 */
namespace render {

// Simple helper function to load an image into a OpenGL texture with common settings
bool load_texture_from_file(const char* filename, GLuint* out_texture, int* out_width,
                            int* out_height);

/**
 * Load image from RGBA array and transfer it into the texture
 * @param data
 * @param width
 * @param height
 * @param out_texture
 */
void load_texture_rgba(const uint32_t* data, int width, int height, GLuint* out_texture);

} // namespace render


/**
 * Functions in this namespace are for dealing with game boy graphics formats
 */
namespace gb {
/*
 * Convert two bytes of a tile (which represent a row of 8 pixels) to the 4 color values
 * available on the game boy.
 * Will return an array of values 0b0, 0b01, 0b10, 0b11, which has to be colorized by a palette.
 */
std::array<uint8_t, 8> convert_tiles(uint8_t byte1, uint8_t byte2);

/**
 * This takes one tile and converts it from the 2bpp format to rgba format.
 * @param tile_data
 * @return
 */
std::array<uint32_t, 64> tile_to_gb_color(std::span<uint8_t, 16> tile_data);

template <typename It>
void map_gb_color_to_rgba(It begin, It end) {
    constexpr std::array<uint32_t, 4> palette = {
        0xff0fbc9b,
        0xff0fac8b,
        0xff306230,
        0xff0f380f,
    };
    std::for_each(begin, end, [palette](auto& x) { x = palette[x]; });
}

/**
 * Convert VRAM tile data content to a 32bit RGBA image by stitching together all tiles from
 * left to right and top to bottom.
 * Tile data is stored from 0x8000-0x97FF. Since each tiles takes up 16 bytes in the gameboys
 * 2bpp encoding, this means 384 tiles, each of which is 8x8 pixels are stored in the tile data
 * It is the callers obligation that image_width_tiles * image_height_tiles == 384.
 * @return width, height of the image in pixels
 */
std::pair<int, int> tile_data_to_image(std::span<uint8_t, 6143> vram,
                                       std::span<uint32_t, 384 * 64> image,
                                       size_t image_width_tiles, size_t image_height_tiles);

} // namespace gb
} // namespace graphics
