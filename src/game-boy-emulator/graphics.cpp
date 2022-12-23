#include <cassert>
#include "graphics.hpp"
#include "bitmanipulation.hpp"

#include "SDL.h"
#include "SDL_opengl.h"

namespace graphics::render {

void load_texture_rgba(const uint32_t* data, int width, int height, GLuint* out_texture) {
    // Create a OpenGL texture identifier
    GLuint image_texture{};
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    *out_texture = image_texture;
}
} // namespace graphics::render


namespace graphics::gb {

std::array<UnmappedColorGb, 8> convert_tile_line(uint8_t byte1, uint8_t byte2) {
    std::array<UnmappedColorGb, 8> pixels{};
    for (uint8_t i = 0; i < 8; ++i) {
        auto msb = bitmanip::bit_value(byte2, i);
        auto lsb = bitmanip::bit_value(byte1, i);
        pixels[7 - i] = static_cast<UnmappedColorGb>((msb << 1) + lsb);
    }
    return pixels;
}

ColorScreen to_screen_color(ColorGb color_gb) {
    return get_screen_palette()[magic_enum::enum_integer(color_gb)];
}

constexpr std::array<ColorScreen, 4> get_screen_palette() {
    constexpr std::array<ColorScreen, 4> palette
        = {ColorScreen::White, ColorScreen::LightGray, ColorScreen::DarkGray, ColorScreen::Black};
    return palette;
}

std::array<UnmappedColorGb, 64> tile_to_gb_color(std::span<uint8_t, 16> tile_data) {
    std::array<UnmappedColorGb, 64> out{};
    for (size_t i = 0; i < 16; i += 2) {
        // 2 bytes represent one 8 pixel wide row in the tile
        auto row = graphics::gb::convert_tile_line(tile_data[i], tile_data[i + 1]);
        for (size_t j = 0; j < row.size(); j++) {
            auto index = (i / 2) * 8 + j;
            out[index] = row[j];
        }
    }
    return out;
}

// std::pair<int, int> tile_data_to_image(std::span<uint8_t> tile_data, Framebuffer& image,
//                                        size_t image_width_tiles, size_t image_height_tiles) {
//     // Every tile is 16 bytes
//     assert(tile_data.size() == image_height_tiles * image_width_tiles * 16
//            && "Tile data size requirement check");
//     // Every tile is 8x8 pixels
//     assert(image.size() == image_width_tiles * image_height_tiles * 64
//            && "Image pixel count requirement check");
//     for (size_t y = 0; y < image_height_tiles; ++y) {
//         for (size_t x = 0; x < image_width_tiles; ++x) {
//             size_t offset = y * image_width_tiles + x;
//             // Take the 16 bytes corresponding to one 8x8 tile
//             auto s = std::span<uint8_t, 16>(tile_data.data() + offset *
//             constants::BYTES_PER_TILE,
//                                             constants::BYTES_PER_TILE);
//             // Convert it to the 4 game boy colors but use uint32 representation to make later in
//             // place conversion to RGBA32 possible.
//             auto tile = graphics::gb::tile_to_gb_color(s);
//             // Copy the tile to the image
//             for (size_t i = 0; i < tile.size(); ++i) {
//                 size_t line_in_tile = i / 8;
//                 auto vram_index = i % 8 + line_in_tile * image_width_tiles * 8;
//                 vram_index += y * 64 * image_width_tiles + x * 8;
//                 image.set_pixel(vram_index, tile[i]);
//             }
//         }
//     }
//     return {image_width_tiles * 8, image_height_tiles * 8};
// }
// void map_gb_color_to_rgba(Framebuffer& image) {
//     const auto palette = get_palette();
//     for (size_t i = 0; i < image.size(); ++i) {
//         image.set_pixel(i, palette[image.get_pixel(i)]);
//     }
// }

TileIndex::TileIndex(size_t tile_width_pixels, size_t tile_height_pixels) :
        m_tile_width(tile_width_pixels), m_tile_height(tile_height_pixels) {}

size_t graphics::gb::TileIndex::pixel_index(size_t x, size_t y) const {
    assert(x < m_tile_width && "TileIndex width out of bounds");
    assert(y < m_tile_height && "TileIndex height out of bounds");
    return x + y * m_tile_width;
}

TileIndexMirrorHorizontal::TileIndexMirrorHorizontal(size_t tile_width_pixels,
                                                     size_t tile_height_pixels) :
        m_tile_width(tile_width_pixels), m_tile_height(tile_height_pixels) {}

size_t TileIndexMirrorHorizontal::pixel_index(size_t x, size_t y) const {
    assert(x < m_tile_width && "TileIndex width out of bounds");
    assert(y < m_tile_height && "TileIndex height out of bounds");
    return m_tile_width - 1 - x + y * m_tile_width;
}

TileIndexMirrorVertical::TileIndexMirrorVertical(size_t tile_width_pixels,
                                                 size_t tile_height_pixels) :
        m_tile_width(tile_width_pixels), m_tile_height(tile_height_pixels) {}


size_t TileIndexMirrorVertical::pixel_index(size_t x, size_t y) const {
    assert(x < m_tile_width && "TileIndex width out of bounds");
    assert(y < m_tile_height && "TileIndex height out of bounds");
    return x + (m_tile_height - 1 - y) * m_tile_width;
}

TileIndexMirrorBothAxes::TileIndexMirrorBothAxes(size_t tile_width_pixels,
                                                               size_t tile_height_pixels) :
        m_tile_width(tile_width_pixels), m_tile_height(tile_height_pixels) {}

size_t TileIndexMirrorBothAxes::pixel_index(size_t x, size_t y) const {
    return m_tile_height * m_tile_width - (x + y * m_tile_width) - 1;

}

} // namespace graphics::gb
