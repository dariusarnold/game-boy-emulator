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

namespace {

constexpr std::array<std::array<graphics::gb::UnmappedColorGb, 8>, 256 * 256> cache{[]() constexpr {
    std::array<std::array<graphics::gb::UnmappedColorGb, 8>, 256 * 256> arr{};
    for (unsigned b1 = 0; b1 <= 255; ++b1) {
        for (unsigned b2 = 0; b2 <= 255; ++b2) {
            auto index = bitmanip::word_from_bytes(b1, b2);
            for (uint8_t i = 0; i < 8; ++i) {
                auto msb = bitmanip::bit_value(b2, i);
                auto lsb = bitmanip::bit_value(b1, i);
                arr[index][7 - i] = static_cast<graphics::gb::UnmappedColorGb>((msb << 1) + lsb);
            }
        }
    }
    return arr;
}()
};

const std::array<graphics::gb::UnmappedColorGb, 8>& get_cached_tile_line(uint8_t byte1, uint8_t byte2) {
    auto index = bitmanip::word_from_bytes(byte1, byte2);
    return cache[index];
}

} // namespace

namespace graphics::gb {

const std::array<UnmappedColorGb, 8>& convert_tile_line(uint8_t byte1, uint8_t byte2) {
   const auto& line = get_cached_tile_line(byte1, byte2);
    return line;
}

namespace {
std::array<ColorScreen, 4> PALETTE
    = {ColorScreen::White, ColorScreen::LightGray, ColorScreen::DarkGray, ColorScreen::Black};
}

ColorScreen to_screen_color(ColorGb color_gb) {
    return PALETTE[magic_enum::enum_integer(color_gb)];
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

std::array<UnmappedColorGb, 128> tile_to_gb_color(std::span<uint8_t, 32> tile_data) {
    std::array<UnmappedColorGb, 128> out{};
    for (size_t i = 0; i < 32; i += 2) {
        // 2 bytes represent one 8 pixel wide row in the tile
        auto row = graphics::gb::convert_tile_line(tile_data[i], tile_data[i + 1]);
        for (size_t j = 0; j < row.size(); j++) {
            auto index = (i / 2) * 8 + j;
            out[index] = row[j];
        }
    }
    return out;
}

TileIndex::TileIndex(size_t tile_width_pixels, size_t tile_height_pixels) :
        m_tile_width(tile_width_pixels), m_tile_height(tile_height_pixels) {}

size_t graphics::gb::TileIndex::pixel_index(size_t x, size_t y) const {
    return x + y * m_tile_width;
}

TileIndexMirrorHorizontal::TileIndexMirrorHorizontal(size_t tile_width_pixels,
                                                     size_t tile_height_pixels) :
        m_tile_width(tile_width_pixels), m_tile_height(tile_height_pixels) {}

size_t TileIndexMirrorHorizontal::pixel_index(size_t x, size_t y) const {
    return m_tile_width - 1 - x + y * m_tile_width;
}

TileIndexMirrorVertical::TileIndexMirrorVertical(size_t tile_width_pixels,
                                                 size_t tile_height_pixels) :
        m_tile_width(tile_width_pixels), m_tile_height(tile_height_pixels) {}


size_t TileIndexMirrorVertical::pixel_index(size_t x, size_t y) const {
    return x + (m_tile_height - 1 - y) * m_tile_width;
}

TileIndexMirrorBothAxes::TileIndexMirrorBothAxes(size_t tile_width_pixels,
                                                 size_t tile_height_pixels) :
        m_tile_width(tile_width_pixels), m_tile_height(tile_height_pixels) {}

size_t TileIndexMirrorBothAxes::pixel_index(size_t x, size_t y) const {
    return m_tile_height * m_tile_width - (x + y * m_tile_width) - 1;
}

} // namespace graphics::gb
