#pragma once

#include "framebuffer.hpp"
#include "graphics.hpp"
#include <memory>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <cassert>

#include "SDL_render.h"
class SDL_Texture;
class SDL_Renderer;


struct SdlTextureDeleter {
    void operator()(SDL_Texture* p) {
        if (p != nullptr) {
            SDL_DestroyTexture(p);
        }
    }
};

/**
 * Couples a framebuffer to SDL rendering.
 */
 template <size_t Width, size_t Height>
class Image {
    size_t m_width = Width;
    size_t m_height = Height;
    std::unique_ptr<SDL_Texture, SdlTextureDeleter> m_texture;

public:
    // Create image with a texture using the given renderer. If no renderer is given, init_texture
    // has to be called before upload_to_texture is used.
    Image(SDL_Renderer* sdl_renderer = nullptr);

    void init_texture(SDL_Renderer* sdl_renderer);

    void upload_to_texture(const Framebuffer<graphics::gb::ColorScreen, Width, Height>& buffer);

    [[nodiscard]] size_t width() const;
    [[nodiscard]] size_t height() const;
    [[nodiscard]] size_t size() const;

    [[nodiscard]] SDL_Texture* get_texture() const;
};

#include "image.tpp"