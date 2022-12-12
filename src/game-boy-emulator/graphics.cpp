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

std::array<uint8_t, 8> convert_tiles(uint8_t byte1, uint8_t byte2) {
    std::array<uint8_t, 8> pixels = {0};
    for (uint8_t i = 0; i < 8; ++i) {
        auto msb = bitmanip::bit_value(byte2, i);
        auto lsb = bitmanip::bit_value(byte1, i);
        pixels[7 - i] = (msb << 1) + lsb;
    }
    return pixels;
}

std::array<uint32_t, 64> tile_to_gb_color(std::span<uint8_t, 16> tile_data) {
    std::array<uint32_t, 64> out{};
    for (size_t i = 0; i < 16; i += 2) {
        // 2 bytes represent one 8 pixel wide row in the tile
        auto row = graphics::gb::convert_tiles(tile_data[i], tile_data[i + 1]);
        for (size_t j = 0; j < row.size(); j++) {
            auto index = (i / 2) * 8 + j;
            out[index] = row[j];
        }
    }
    return out;
}

std::pair<int, int> tile_data_to_image(std::span<uint8_t, 8192> vram,
                                       std::span<uint32_t, 384 * 64> image,
                                       size_t image_width_tiles, size_t image_height_tiles) {
    for (size_t y = 0; y < image_height_tiles; ++y) {
        for (size_t x = 0; x < image_width_tiles; ++x) {
            size_t offset = y * image_width_tiles + x;
            // Take the 16 bytes corresponding to one 8x8 tile
            auto s = std::span<uint8_t, 16>(vram.data() + offset * 16, 16);
            // Convert it to the 4 game boy colors but use uint32 representation to make later in
            // place conversion to RGBA32 possible.
            auto tile = graphics::gb::tile_to_gb_color(s);
            // Copy the tile to the image
            for (size_t i = 0; i < tile.size(); ++i) {
                size_t line_in_tile = i / 8;
                auto vram_index = i % 8 + line_in_tile * image_width_tiles * 8;
                vram_index += y * 64 * image_width_tiles + x * 8;
                image[vram_index] = tile[i];
            }
        }
    }
    return {image_width_tiles * 8, image_height_tiles * 8};
}
} // namespace graphics::gb
