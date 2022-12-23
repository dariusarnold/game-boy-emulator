#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

/**
 * x axes: horizontal (corresponds to width)
 * y axes: vertical (corresponds to height)
 * origin top left
 */
template <typename PixelType>
class Framebuffer {

    size_t m_width;
    size_t m_height;
    std::vector<PixelType> m_buffer;

    // Get the index a pixel would have in a 1D representation of the image data
    [[nodiscard]] size_t pixel_index(size_t x, size_t y) const;

public:
    using pixel_type_t = PixelType;

    Framebuffer(size_t width, size_t height, PixelType fill = {});

    [[nodiscard]] PixelType get_pixel(size_t x, size_t y) const;
    [[nodiscard]] PixelType get_pixel(size_t pixel_index) const;
    void set_pixel(size_t x, size_t y, PixelType color);
    void set_pixel(size_t pixel_index, PixelType color);

    // Sets a pixel within the framebuffer, but a coordinate outside of the buffer will be mapped
    // into the buffer.
    void set_pixel_wraparound(int x, int y, PixelType color);
    [[nodiscard]] PixelType get_pixel_wraparound(int x, int y) const;

    // Transfer frame buffer content into another buffer.
    void copy_into(void* ptr) const;

    [[nodiscard]] size_t width() const;
    [[nodiscard]] size_t height() const;
    [[nodiscard]] size_t size() const;

    // Clear screen buffer
    void reset(PixelType fill = {});
};

template <typename PixelType>
void draw_rectangle_border(Framebuffer<PixelType>& img, int top_left_x, int top_left_y,
                           int rect_width, int rect_height, PixelType rect_color);

#include "framebuffer.tpp"
