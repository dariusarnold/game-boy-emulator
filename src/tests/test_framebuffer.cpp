#include "catch2/catch.hpp"

#include "framebuffer.hpp"

TEST_CASE("X/Y index access") {
    Framebuffer<int> i{4, 3};
    auto c = 0xFF00FF00;
    i.set_pixel(1, 3, c);
    auto d = i.get_pixel(1, 3);
    CHECK(c == d);
}

TEST_CASE("pixel index access") {
    Framebuffer<int> i{4, 3};
    auto c = 0xFF00FF00;
    i.set_pixel(7, c);
    auto d = i.get_pixel(7);
    CHECK(c == d);
}

TEST_CASE("Info getters") {
    Framebuffer<int> i{4, 3};
    CHECK(i.size() == 12);
    CHECK(i.width() == 4);
    CHECK(i.height() == 3);
}