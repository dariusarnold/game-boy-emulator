#include <cstring>
#include <cstdlib>

template <typename PixelType, size_t Width, size_t Height>
Framebuffer<PixelType, Width, Height>::Framebuffer(PixelType fill) :
        m_buffer(Width * Height, fill) {}

template <typename PixelType, size_t Width, size_t Height>
size_t Framebuffer<PixelType, Width, Height>::pixel_index(size_t x, size_t y) const {
    return x + y * m_width;
}

template <typename PixelType, size_t Width, size_t Height>
PixelType Framebuffer<PixelType, Width, Height>::get_pixel(size_t x, size_t y) const {
    return m_buffer[pixel_index(x, y)];
}

template <typename PixelType, size_t Width, size_t Height>
void Framebuffer<PixelType, Width, Height>::set_pixel(size_t x, size_t y, PixelType color) {
    auto i = pixel_index(x, y);
    m_buffer[i] = color;
}

template <typename PixelType, size_t Width, size_t Height>
size_t Framebuffer<PixelType, Width, Height>::width() const {
    return m_width;
}

template <typename PixelType, size_t Width, size_t Height>
size_t Framebuffer<PixelType, Width, Height>::height() const {
    return m_height;
}

template <typename PixelType, size_t Width, size_t Height>
size_t Framebuffer<PixelType, Width, Height>::size() const {
    return m_buffer.size();
}

template <typename PixelType, size_t Width, size_t Height>
void Framebuffer<PixelType, Width, Height>::set_pixel(size_t pixel_index, PixelType color) {
    m_buffer[pixel_index] = color;
}

template <typename PixelType, size_t Width, size_t Height>
PixelType Framebuffer<PixelType, Width, Height>::get_pixel(size_t pixel_index) const {
    return m_buffer[pixel_index];
}

template <typename PixelType, size_t Width, size_t Height>
void Framebuffer<PixelType, Width, Height>::copy_into(void* ptr) const {
    std::memcpy(ptr, m_buffer.data(), sizeof(PixelType) * m_buffer.size());
}

template <typename PixelType, size_t Width, size_t Height>
void Framebuffer<PixelType, Width, Height>::take_from(void* ptr) {
    std::memcpy(m_buffer.data(), ptr, sizeof(PixelType) * m_buffer.size());
}

template <typename PixelType, size_t Width, size_t Height>
void Framebuffer<PixelType, Width, Height>::reset(PixelType fill) {
    m_buffer.assign(size(), fill);
}

template <typename PixelType, size_t Width, size_t Height>
void Framebuffer<PixelType, Width, Height>::set_pixel_wraparound(int x, int y, PixelType color) {

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

template <typename PixelType, size_t Width, size_t Height>
PixelType Framebuffer<PixelType, Width, Height>::get_pixel_wraparound(int x, int y) const {

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

template <typename PixelType, size_t Width, size_t Height>
bool Framebuffer<PixelType, Width, Height>::operator==(
    const Framebuffer<PixelType, Width, Height>& other) const {
    return other.m_buffer == m_buffer;
}

template <typename PixelType, size_t Width, size_t Height>
SDL_Surface* Framebuffer<PixelType, Width, Height>::to_surface() {
    return SDL_CreateRGBSurfaceFrom(m_buffer.data(), width(), height(), sizeof(PixelType) * 8,
                                    width() * sizeof(PixelType), 0x00000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
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
template <typename PixelType, size_t Width, size_t Height>
void draw_rectangle_border(Framebuffer<PixelType, Width, Height>& img, int top_left_x,
                           int top_left_y, int rect_width, int rect_height, PixelType rect_color) {
    for (auto x = top_left_x; x < top_left_x + rect_width; ++x) {
        img.set_pixel_wraparound(x, top_left_y, rect_color);
        img.set_pixel_wraparound(x, top_left_y + rect_height - 1, rect_color);
    }
    for (auto y = top_left_y; y < top_left_y + rect_height; ++y) {
        img.set_pixel_wraparound(top_left_x, y, rect_color);
        img.set_pixel_wraparound(top_left_x + rect_width - 1, y, rect_color);
    }
}
