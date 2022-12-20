#include "bitmanipulation.hpp"
#include <catch2/catch.hpp>

#include "joypad.hpp"

TEST_CASE("Register FF00") {
    Joypad jp(nullptr);

    SECTION("Default value is all keys released") {
        CHECK(jp.read_byte() == 0b00001111);
    }

    SECTION("Press a key when not requesting any key type") {
        jp.press_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00001111);
    }

    SECTION("Press a key when requesting the other key type") {
        jp.write_byte(0b00010000);
        jp.press_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00011111);
        jp.release_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00011111);
    }

    SECTION("Press/release a key when requesting the matching key type") {
        jp.write_byte(0b00100000);
        jp.press_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00101110);
        jp.release_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00101111);
    }
}