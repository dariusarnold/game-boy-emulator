#include "SDL.h"
#include <cassert>
#include <cstring>

template <size_t Width, size_t Height>
Image<Width, Height>::Image(SDL_Renderer* sdl_renderer) : m_texture(nullptr) {
    if (sdl_renderer != nullptr) {
        auto* texture
            = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
                                static_cast<int>(m_width), static_cast<int>(m_height));
        m_texture = std::unique_ptr<SDL_Texture, SdlTextureDeleter>{texture};
    }
}

template <size_t Width, size_t Height>
void Image<Width, Height>::upload_to_texture(
    const Framebuffer<graphics::gb::ColorScreen, Width, Height>& buffer) {
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

template <size_t Width, size_t Height>
void Image<Width, Height>::init_texture(SDL_Renderer* sdl_renderer) {
    auto* texture
        = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
                            static_cast<int>(m_width), static_cast<int>(m_height));
    m_texture = std::unique_ptr<SDL_Texture, SdlTextureDeleter>(texture);
}

template <size_t Width, size_t Height>
SDL_Texture* Image<Width, Height>::get_texture() const {
    return m_texture.get();
}

template <size_t Width, size_t Height>
size_t Image<Width, Height>::width() const {
    return m_width;
}

template <size_t Width, size_t Height>
size_t Image<Width, Height>::height() const {
    return m_height;
}

template <size_t Width, size_t Height>
size_t Image<Width, Height>::size() const {
    return m_width * m_height;
}

template <size_t Width, size_t Height>
void Image<Width, Height>::save_as_bitmap(std::string_view filename) const {
    SDL_Surface* surf = nullptr;
    auto res = SDL_LockTextureToSurface(m_texture.get(), nullptr, &surf);
    assert(res == 0);
    res = SDL_SaveBMP(surf, filename.data());
    assert(res == 0);
}
