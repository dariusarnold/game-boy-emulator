#include "graphics.hpp"
#include "framebuffer.hpp"
#include <catch2/catch.hpp>
#include "fmt/format.h"


TEST_CASE("Tile conversion1") {
    auto result = graphics::gb::convert_tile_line(0x3C, 0x7E);
    std::array<graphics::gb::UnmappedColorGb, 8> expected_result{
        graphics::gb::UnmappedColorGb ::Color0, graphics::gb::UnmappedColorGb ::Color2,
        graphics::gb::UnmappedColorGb ::Color3, graphics::gb::UnmappedColorGb ::Color3,
        graphics::gb::UnmappedColorGb ::Color3, graphics::gb::UnmappedColorGb ::Color3,
        graphics::gb::UnmappedColorGb ::Color2, graphics::gb::UnmappedColorGb ::Color0};
    CHECK(result == expected_result);
}

TEST_CASE("Tile conversion2") {
    auto result = graphics::gb::convert_tile_line(0x7C, 0x56);
    std::array<graphics::gb::UnmappedColorGb, 8> expected_result
        = {graphics::gb::UnmappedColorGb::Color0, graphics::gb::UnmappedColorGb::Color3,
           graphics::gb::UnmappedColorGb::Color1, graphics::gb::UnmappedColorGb::Color3,
           graphics::gb::UnmappedColorGb::Color1, graphics::gb::UnmappedColorGb::Color3,
           graphics::gb::UnmappedColorGb::Color2, graphics::gb::UnmappedColorGb::Color0};
    CHECK(result == expected_result);
}

TEST_CASE("Tile conversion complete") {
    std::array<uint8_t, 16> tile{0x7C, 0x7C, 0x00, 0xC6, 0xC6, 0x00, 0x00, 0xFE,
                                 0xC6, 0xC6, 0x00, 0xC6, 0xC6, 0x00, 0x00, 0x00};
    using namespace graphics::gb;
    std::array<std::array<UnmappedColorGb, 8>, 8> expected_res{
        std::array<UnmappedColorGb, 8>{UnmappedColorGb::Color0, UnmappedColorGb::Color3,
                                       UnmappedColorGb::Color3, UnmappedColorGb::Color3,
                                       UnmappedColorGb::Color3, UnmappedColorGb::Color3,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color0},
        std::array<UnmappedColorGb, 8>{UnmappedColorGb::Color2, UnmappedColorGb::Color2,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color0,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color2,
                                       UnmappedColorGb::Color2, UnmappedColorGb::Color0},
        std::array<UnmappedColorGb, 8>{UnmappedColorGb::Color1, UnmappedColorGb::Color1,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color0,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color1,
                                       UnmappedColorGb::Color1, UnmappedColorGb::Color0},
        std::array<UnmappedColorGb, 8>{UnmappedColorGb::Color2, UnmappedColorGb::Color2,
                                       UnmappedColorGb::Color2, UnmappedColorGb::Color2,
                                       UnmappedColorGb::Color2, UnmappedColorGb::Color2,
                                       UnmappedColorGb::Color2, UnmappedColorGb::Color0},
        std::array<UnmappedColorGb, 8>{UnmappedColorGb::Color3, UnmappedColorGb::Color3,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color0,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color3,
                                       UnmappedColorGb::Color3, UnmappedColorGb::Color0},
        std::array<UnmappedColorGb, 8>{UnmappedColorGb::Color2, UnmappedColorGb::Color2,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color0,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color2,
                                       UnmappedColorGb::Color2, UnmappedColorGb::Color0},
        std::array<UnmappedColorGb, 8>{UnmappedColorGb::Color1, UnmappedColorGb::Color1,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color0,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color1,
                                       UnmappedColorGb::Color1, UnmappedColorGb::Color0},
        std::array<UnmappedColorGb, 8>{UnmappedColorGb::Color0, UnmappedColorGb::Color0,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color0,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color0,
                                       UnmappedColorGb::Color0, UnmappedColorGb::Color0}};
    auto i = GENERATE(range(0, 8));
    auto res = graphics::gb::convert_tile_line(tile[i * 2], tile[i * 2 + 1]);
    INFO(fmt::format("Iteration {}", i));
    CHECK(res == expected_res[i]);
}

TEST_CASE("Complete tile conversion example") {
    std::array<uint8_t, 16> tile{0xFF, 0x00, 0x7E, 0xFF, 0x85, 0x81, 0x89, 0x83,
                                 0x93, 0x85, 0xA5, 0x8B, 0xC9, 0x97, 0x7E, 0xFF};
    std::span<uint8_t, 16> s{tile.data(), tile.size()};
    auto res = graphics::gb::tile_to_gb_color(s);
    // clang-format off
    std::array<uint32_t, 64> expected_res{1,1,1,1,1,1,1,1,
    2,3,3,3,3,3,3,2,
    3,0,0,0,0,1,0,3,
    3,0,0,0,1,0,2,3,
    3,0,0,1,0,2,1,3,
    3,0,1,0,2,1,2,3,
    3,1,0,2,1,2,2,3,
    2,3,3,3,3,3,3,2};
    // clang-format on
    for (auto i = 0; i < expected_res.size(); ++i) {
        CHECK(res[i] == static_cast<graphics::gb::UnmappedColorGb>(expected_res[i]));
    }
}

TEST_CASE("Convert tile line") {
    auto line = graphics::gb::convert_tile_line(0x3C, 0xCC);
    auto expected = std::vector{2, 2, 1, 1, 3, 3, 0, 0};
    for (size_t i = 0; i < line.size(); ++i) {
        CHECK(line[i] == static_cast<graphics::gb::UnmappedColorGb>(expected[i]));
    }
}

TEST_CASE("Normal tile index") {
    using namespace graphics::gb;

    TileIndex ti{4, 3};

    CHECK(ti.pixel_index(0, 0) == 0);
    CHECK(ti.pixel_index(2, 0) == 2);
    CHECK(ti.pixel_index(0, 1) == 4);
    CHECK(ti.pixel_index(1, 1) == 5);
    CHECK(ti.pixel_index(3, 2) == 11);
}

TEST_CASE("Horizontally mirrored tile index") {
    using namespace graphics::gb;

    TileIndexMirrorHorizontal tih{4, 3};

    /**
     * 00 01 02 03
     * 04 05 06 07
     * 08 09 10 11
     * would be mirrored to
     * 03 02 01 00
     * 07 06 05 04
     * 11 10 09 08
     *
     */

    CHECK(tih.pixel_index(0, 0) == 3);
    CHECK(tih.pixel_index(2, 0) == 1);
    CHECK(tih.pixel_index(0, 1) == 7);
    CHECK(tih.pixel_index(1, 1) == 6);
    CHECK(tih.pixel_index(3, 2) == 8);
}

TEST_CASE("Vertically mirrored tile index") {
    using namespace graphics::gb;

    TileIndexMirrorVertical tiv{4, 3};

    /**
     * 00 01 02 03
     * 04 05 06 07
     * 08 09 10 11
     * would be mirrored to
     * 08 09 10 11
     * 04 05 06 07
     * 00 01 02 03
     *
     */

    CHECK(tiv.pixel_index(0, 0) == 8);
    CHECK(tiv.pixel_index(2, 0) == 10);
    CHECK(tiv.pixel_index(0, 1) == 4);
    CHECK(tiv.pixel_index(1, 1) == 5);
    CHECK(tiv.pixel_index(3, 2) == 3);
}

TEST_CASE("Using mirrored TileIndex to access Framebuffer") {
    Framebuffer<int> fb(2, 2);
    // Fill with
    // 1 2
    // 3 4
    fb.set_pixel(0, 0, 1);
    fb.set_pixel(1, 0, 2);
    fb.set_pixel(0, 1, 3);
    fb.set_pixel(1, 1, 4);
    using namespace graphics::gb;

    SECTION("No mirroring") {
        TileIndex ti(2, 2);
        CHECK(fb.get_pixel(ti.pixel_index(0, 0)) == 1);
        CHECK(fb.get_pixel(ti.pixel_index(1, 0)) == 2);
        CHECK(fb.get_pixel(ti.pixel_index(0, 1)) == 3);
        CHECK(fb.get_pixel(ti.pixel_index(1, 1)) == 4);
    }

    SECTION("Mirrored horizontally") {
        TileIndexMirrorHorizontal tih(2, 2);
        CHECK(fb.get_pixel(tih.pixel_index(0, 0)) == 2);
        CHECK(fb.get_pixel(tih.pixel_index(1, 0)) == 1);
        CHECK(fb.get_pixel(tih.pixel_index(0, 1)) == 4);
        CHECK(fb.get_pixel(tih.pixel_index(1, 1)) == 3);
    }

    SECTION("Mirrored vertically") {
        TileIndexMirrorVertical tiv(2, 2);
        CHECK(fb.get_pixel(tiv.pixel_index(0, 0)) == 3);
        CHECK(fb.get_pixel(tiv.pixel_index(1, 0)) == 4);
        CHECK(fb.get_pixel(tiv.pixel_index(0, 1)) == 1);
        CHECK(fb.get_pixel(tiv.pixel_index(1, 1)) == 2);
    }
}