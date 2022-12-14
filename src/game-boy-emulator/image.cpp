#include "image.hpp"

#include "SDL.h"
#include <cassert>
#include <cstring>


Image::Image(size_t width, size_t height, SDL_Renderer* sdl_renderer, uint32_t fill) :
        m_width(width), m_height(height), m_buffer(width * height, fill), m_texture(nullptr) {
    if (sdl_renderer != nullptr) {
        auto* texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA32,
                                          SDL_TEXTUREACCESS_STREAMING, width, height);
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
    void* pixels;
    int pitch;
    SDL_LockTexture(m_texture.get(), nullptr, &pixels, &pitch);
    std::memcpy(pixels, m_buffer.data(), sizeof(uint32_t) * m_buffer.size());
    SDL_UnlockTexture(m_texture.get());
}

void Image::init_texture(SDL_Renderer* sdl_renderer) {
    auto* texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA32,
                                      SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
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
