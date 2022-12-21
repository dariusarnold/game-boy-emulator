#include "image.hpp"

#include "SDL.h"
#include <cassert>
#include <cstring>


Image::Image(size_t width, size_t height, SDL_Renderer* sdl_renderer) :
        m_width(width), m_height(height), m_texture(nullptr) {
    if (sdl_renderer != nullptr) {
        auto* texture
            = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
                                static_cast<int>(width), static_cast<int>(height));
        m_texture = std::unique_ptr<SDL_Texture, SdlTextureDeleter>{texture};
    }
}

void Image::upload_to_texture(const Framebuffer<graphics::gb::ColorScreen>& buffer) {
    assert(m_texture != nullptr && "Call init_texture or pass SDL_Renderer in constructor");
    void* pixels = nullptr;
    int pitch = 0;
    auto rc = SDL_LockTexture(m_texture.get(), nullptr, &pixels, &pitch);
    (void)rc;
    assert(rc == 0 && "Failed to lock texture");
    assert(pitch == int(buffer.width() * sizeof(graphics::gb::ColorScreen))
           && "Pitch size assumption error");
    buffer.copy_into(pixels);
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
    return m_width * m_height;
}
