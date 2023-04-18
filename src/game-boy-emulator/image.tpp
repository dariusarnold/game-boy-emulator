#include <cassert>
#include <cstring>

#include <GL/gl.h>

template <size_t Width, size_t Height>
Image<Width, Height>::Image() : m_texture{} {}

template <size_t Width, size_t Height>
void Image<Width, Height>::upload_to_texture(
    const Framebuffer<graphics::gb::ColorScreen, Width, Height>& buffer) {
    assert(m_texture != GLuint{} && "Call init_texture before using the Image");

// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 buffer.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

template <size_t Width, size_t Height>
void Image<Width, Height>::init_texture() {
    // Create a OpenGL texture identifier
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#ifdef __EMSCRIPTEN__
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
#endif
    glBindTexture(GL_TEXTURE_2D, 0);
}

template <size_t Width, size_t Height>
GLuint Image<Width, Height>::get_texture() const {
    return m_texture;
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

// template <size_t Width, size_t Height>
// void Image<Width, Height>::save_as_bitmap(std::string_view filename) const {
//     SDL_Surface* surf = nullptr;
//     auto res = SDL_LockTextureToSurface(m_texture.get(), nullptr, &surf);
//     assert(res == 0);
//     res = SDL_SaveBMP(surf, filename.data());
//     assert(res == 0);
// }
