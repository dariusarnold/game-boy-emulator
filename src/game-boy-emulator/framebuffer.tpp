#include "framebuffer.hpp"
#include <cstring>
#include <cstdlib>

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

template <typename PixelType>
void Framebuffer<PixelType>::reset(PixelType fill) {
    m_buffer.assign(size(), fill);
}

template <typename PixelType>
void Framebuffer<PixelType>::set_pixel_wraparound(int x, int y, PixelType color) {

    auto x_unsigned = [&] {
        if (x < 0) {
            return static_cast<int>(m_width) - (std::abs(x) % static_cast<int>(m_width));
        }
        return x % static_cast<int>(m_width);
    }();
    auto y_unsigned = [&] {
        if (y < 0) {
            return static_cast<int>(m_height) - (std::abs(y) % static_cast<int>(m_height));
        }
        return y % static_cast<int>(m_height);
    }();
    set_pixel(static_cast<size_t>(x_unsigned), static_cast<size_t>(y_unsigned), color);
}

template <typename PixelType>
PixelType Framebuffer<PixelType>::get_pixel_wraparound(int x, int y) const {

    auto x_unsigned = [&] {
        if (x < 0) {
            return static_cast<int>(m_width) - (std::abs(x) % static_cast<int>(m_width));
        }
        return x % static_cast<int>(m_width);
    }();
    auto y_unsigned = [&] {
        if (y < 0) {
            return static_cast<int>(m_height) - (std::abs(y) % static_cast<int>(m_height));
        }
        return y % static_cast<int>(m_height);
    }();
    return get_pixel(static_cast<size_t>(x_unsigned), static_cast<size_t>(y_unsigned));
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
void draw_rectangle_border(Framebuffer<PixelType>& img, int top_left_x, int top_left_y,
                           int rect_width, int rect_height, PixelType rect_color) {
    for (auto x = top_left_x; x < top_left_x + rect_width; ++x) {
        img.set_pixel_wraparound(x, top_left_y, rect_color);
        img.set_pixel_wraparound(x, top_left_y + rect_height - 1, rect_color);
    }
    for (auto y = top_left_y; y < top_left_y + rect_height; ++y) {
        img.set_pixel_wraparound(top_left_x, y, rect_color);
        img.set_pixel_wraparound(top_left_x + rect_width - 1, y, rect_color);
    }
}
