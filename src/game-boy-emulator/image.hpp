#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include <cstddef>

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
 * x axes: horizontal (corresponds to width)
 * y axes: vertical (corresponds to height)
 * origin top left
 */
class Image {
    size_t m_width;
    size_t m_height;
    std::vector<uint32_t> m_buffer;
    std::unique_ptr<SDL_Texture, SdlTextureDeleter> m_texture;

    // Get the index a pixel would have in a 1D representation of the image data
    [[nodiscard]] size_t pixel_index(size_t x, size_t y) const;

public:
    // Create image with a texture using the given renderer. If no renderer is given, init_texture
    // has to be called before upload_to_texture is used.
    Image(size_t width, size_t height, SDL_Renderer* sdl_renderer = nullptr,
          uint32_t fill = 0xFFFFFF01);

    void init_texture(SDL_Renderer* sdl_renderer);

    [[nodiscard]] uint32_t get_pixel(size_t x, size_t y) const;
    [[nodiscard]] uint32_t get_pixel(size_t pixel_index) const;
    void set_pixel(size_t x, size_t y, uint32_t color);
    void set_pixel(size_t pixel_index, uint32_t color);

    [[nodiscard]] size_t width() const;
    [[nodiscard]] size_t height() const;
    [[nodiscard]] size_t size() const;

    void upload_to_texture();

    [[nodiscard]] SDL_Texture* get_texture() const;
};

void draw_rectangle(Image& img, )