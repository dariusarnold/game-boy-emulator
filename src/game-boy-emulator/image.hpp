#pragma once

#include "framebuffer.hpp"
#include "graphics.hpp"
#include <memory>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <cassert>

template <typename PixelType, size_t Width, size_t Height>
class Framebuffer;


/**
 * Couples a framebuffer to SDL rendering.
 */
template <size_t Width, size_t Height>
class Image {
    size_t m_width = Width;
    size_t m_height = Height;
    GLuint m_texture;

public:
    // Create image with a texture using the given renderer. If no renderer is given, init_texture
    // has to be called before upload_to_texture is used.
    explicit Image();

    void init_texture();

    void upload_to_texture(const Framebuffer<graphics::gb::ColorScreen, Width, Height>& buffer);

    [[nodiscard]] size_t width() const;
    [[nodiscard]] size_t height() const;
    [[nodiscard]] size_t size() const;

    [[nodiscard]] GLuint get_texture() const;

    void save_as_bitmap(std::string_view filename) const;
};

#include "image.tpp"