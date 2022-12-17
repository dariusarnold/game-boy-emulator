#include "image.hpp"

#include "SDL.h"
#include <cassert>
#include <cstring>


Image::Image(size_t width, size_t height, SDL_Renderer* sdl_renderer, uint32_t fill) :
        m_width(width), m_height(height), m_buffer(width * height, fill), m_texture(nullptr) {
    if (sdl_renderer != nullptr) {
        auto* texture
            = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
                                static_cast<int>(width), static_cast<int>(height));
        m_texture = std::unique_ptr<SDL_Texture, SdlTextureDeleter>{texture};
    }
}

size_t Image::pixel_index(size_t x, size_t y) const {
    return x + y * m_width;
}

uint32_t Image::get_pixel(size_t x, size_t y) const {
    return m_buffer[pixel_index(x, y)];
}

void Image::set_pixel(size_t x, size_t y, uint32_t color) {
    m_buffer[pixel_index(x, y)] = color;
}

void Image::upload_to_texture() {
    assert(m_texture != nullptr && "Call init_texture or pass SDL_Renderer in constructor");
    void* pixels = nullptr;
    int pitch = 0;
    SDL_LockTexture(m_texture.get(), nullptr, &pixels, &pitch);
    std::memcpy(pixels, m_buffer.data(), sizeof(uint32_t) * m_buffer.size());
    SDL_UnlockTexture(m_texture.get());
}

void Image::init_texture(SDL_Renderer* sdl_renderer) {
    auto* texture
        = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
                            static_cast<int>(m_width), static_cast<int>(m_height));
    m_texture = std::unique_ptr<SDL_Texture, SdlTextureDeleter>(texture);
}

SDL_Texture* Image::get_texture() const {
    return m_texture.get();
}

size_t Image::width() const {
    return m_width;
}

size_t Image::height() const {
    return m_height;
}

size_t Image::size() const {
    return m_buffer.size();
}

void Image::set_pixel(size_t pixel_index, uint32_t color) {
    m_buffer[pixel_index] = color;
}

uint32_t Image::get_pixel(size_t pixel_index) const {
    return m_buffer[pixel_index];
}

/**
 * Draw an unfilled rectangle with 1 pixel wide border on the image
 * @param img
 * @param top_left_x
 * @param top_left_y
 * @param rect_width
 * @param rect_height
 * @param rect_color
 */
void draw_rectangle_border(Image& img, size_t top_left_x, size_t top_left_y, size_t rect_width,
                           size_t rect_height, uint32_t rect_color) {
    for (size_t x = top_left_x; x < top_left_x + rect_width; ++x) {
        img.set_pixel(x, top_left_y, rect_color);
        img.set_pixel(x, top_left_y + rect_height - 1, rect_color);
    }
    for (size_t y = top_left_y; y < top_left_y + rect_height; ++y) {
        img.set_pixel(top_left_x, y, rect_color);
        img.set_pixel(top_left_x + rect_width - 1, y, rect_color);
    }
}
