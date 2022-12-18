#include <cstring>
#include "framebuffer.hpp"

template <typename PixelType>
Framebuffer<PixelType>::Framebuffer(size_t width, size_t height, PixelType fill) :
        m_width(width), m_height(height), m_buffer(width * height, fill) {}

template <typename PixelType>
size_t Framebuffer<PixelType>::pixel_index(size_t x, size_t y) const {
    return x + y * m_width;
}

template <typename PixelType>
PixelType Framebuffer<PixelType>::get_pixel(size_t x, size_t y) const {
    return m_buffer[pixel_index(x, y)];
}

template <typename PixelType>
void Framebuffer<PixelType>::set_pixel(size_t x, size_t y, PixelType color) {
    m_buffer[pixel_index(x, y)] = color;
}

template <typename PixelType>
size_t Framebuffer<PixelType>::width() const {
    return m_width;
}

template <typename PixelType>
size_t Framebuffer<PixelType>::height() const {
    return m_height;
}

template <typename PixelType>
size_t Framebuffer<PixelType>::size() const {
    return m_buffer.size();
}

template <typename PixelType>
void Framebuffer<PixelType>::set_pixel(size_t pixel_index, PixelType color) {
    m_buffer[pixel_index] = color;
}

template <typename PixelType>
PixelType Framebuffer<PixelType>::get_pixel(size_t pixel_index) const {
    return m_buffer[pixel_index];
}

template <typename PixelType>
void Framebuffer<PixelType>::copy_into(void* ptr) const {
    std::memcpy(ptr, m_buffer.data(), sizeof(PixelType) * m_buffer.size());
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
template <typename PixelType>
void draw_rectangle_border(Framebuffer<PixelType>& img, size_t top_left_x, size_t top_left_y,
                           size_t rect_width, size_t rect_height, PixelType rect_color) {
    for (size_t x = top_left_x; x < top_left_x + rect_width; ++x) {
        img.set_pixel(x, top_left_y, rect_color);
        img.set_pixel(x, top_left_y + rect_height - 1, rect_color);
    }
    for (size_t y = top_left_y; y < top_left_y + rect_height; ++y) {
        img.set_pixel(top_left_x, y, rect_color);
        img.set_pixel(top_left_x + rect_width - 1, y, rect_color);
    }
}
