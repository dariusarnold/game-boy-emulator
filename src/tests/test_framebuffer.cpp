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

TEST_CASE("Wrapped pixel access") {
    Framebuffer<int> fb{4, 3};
    // Fill pixels ascending, buffer will look like this:
    // 00 01 02 03
    // 04 05 06 07
    // 08 09 10 11
    for (auto i = 0; i < fb.size(); ++i) {
        fb.set_pixel(i, i);
    }

    SECTION("Wrap right") {
        fb.set_pixel_wraparound(5, 1, 99);
        CHECK(fb.get_pixel(1, 1) == 99);
    }

    SECTION("Wrap bottom") {
        fb.set_pixel_wraparound(1, 5, 99);
        CHECK(fb.get_pixel(1, 2) == 99);
    }

    SECTION("Wrap left") {
        fb.set_pixel_wraparound(-1, 1, 99);
        CHECK(fb.get_pixel(3, 1) == 99);
    }

    SECTION("Wrap top") {
        fb.set_pixel_wraparound(2, -2, 99);
        CHECK(fb.get_pixel(2, 1) == 99);
    }

    SECTION("Wrap around two axes") {
        fb.set_pixel_wraparound(5, -2, 99);
        CHECK(fb.get_pixel(1, 1) == 99);
    }

    SECTION("Multiple wraparound positive") {
        fb.set_pixel_wraparound(9, 1, 99);
        CHECK(fb.get_pixel(1, 1) == 99);
    }

    SECTION("Multiple wraparound negative") {
        fb.set_pixel_wraparound(-9, 1, 99);
        CHECK(fb.get_pixel(3, 1) == 99);
    }
}