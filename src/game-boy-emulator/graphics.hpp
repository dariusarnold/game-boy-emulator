#pragma once

#include "SDL_opengl.h"
#include "magic_enum.hpp"

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

// Colors not yet converted by a palette.
enum class UnmappedColorGb : uint8_t {
    Color0 = 0,
    Color1 = 1,
    Color2 = 2,
    Color3 = 3,
};

// Colors after a palette was applied to their values
enum class ColorGb : uint8_t {
    White = 0,
    LightGray = 1,
    DarkGray = 2,
    Black = 3,
};

// Color values used for SDL rendering
enum class ColorScreen : uint32_t {
    // Format is ARGB
    TrueWhite = 0xFFFFFFFF,
    White = 0xff0fbc9b,
    LightGray = 0xff0fac8b,
    DarkGray = 0xff306230,
    Black = 0xff0f380f,
    Highlight = 0x00ffff,
};

ColorScreen to_screen_color(ColorGb color_gb);

/*
 * Convert two bytes of a tile (which represent a row of 8 pixels) to the 4 color values
 * available on the game boy.
 * Will return an array of values 0b0, 0b01, 0b10, 0b11, which has to be colorized by a palette.
 */
const std::array<UnmappedColorGb, 8>& convert_tile_line(uint8_t byte1, uint8_t byte2);

/**
 * This takes one tile and converts it from the 2bpp format to rgba format.
 * @param tile_data
 * @return
 */
std::array<UnmappedColorGb, 64> tile_to_gb_color(std::span<uint8_t, 16> tile_data);
std::array<UnmappedColorGb, 128> tile_to_gb_color(std::span<uint8_t, 32> tile_data);

/**
 * Get index into a 1D array used to store 2D image by x,y coordinate.
 * 1D array consists of pixels in row-major order.
 */
class TileIndex {
    size_t m_tile_width;
    [[maybe_unused]] size_t m_tile_height;

public:
    TileIndex(size_t tile_width_pixels, size_t tile_height_pixels);
    [[nodiscard]] size_t pixel_index(size_t x, size_t y) const;
};

/**
 * Get index into a 1D array used to store 2D image by x,y coordinate.
 * Treat the image as horizontally mirrored, meaning when requesting a x/y coordinate return the
 * index required to access the pixel in the mirrored image while assuming storage still stays
 * linear and in row-major order.
 */
class TileIndexMirrorHorizontal {
    size_t m_tile_width;
    [[maybe_unused]] size_t m_tile_height;

public:
    TileIndexMirrorHorizontal(size_t tile_width_pixels, size_t tile_height_pixels);
    [[nodiscard]] size_t pixel_index(size_t x, size_t y) const;
};

/**
 * Get index into a 1D array used to store 2D image by x,y coordinate.
 * Treat the image as verticall mirrored, meaning when requesting a x/y coordinate return the
 * index required to access the pixel in the mirrored image while assuming storage still stays
 * linear and in row-major order.
 */
class TileIndexMirrorVertical {
    size_t m_tile_width;
    size_t m_tile_height;

public:
    TileIndexMirrorVertical(size_t tile_width_pixels, size_t tile_height_pixels);
    [[nodiscard]] size_t pixel_index(size_t x, size_t y) const;
};

class TileIndexMirrorBothAxes {
    size_t m_tile_width;
    size_t m_tile_height;

public:
    TileIndexMirrorBothAxes(size_t tile_width_pixels, size_t tile_height_pixels);
    [[nodiscard]] size_t pixel_index(size_t x, size_t y) const;
};


} // namespace gb
} // namespace graphics
