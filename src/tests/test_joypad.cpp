#include "bitmanipulation.hpp"
#include <catch2/catch.hpp>

#include "joypad.hpp"

namespace {
uint8_t REQUEST_ACTION_BUTTONS = 0b011111;
uint8_t REQUEST_DIRECTION_BUTTONS = 0b101111;
uint8_t REQUEST_BOTH_TYPES = 0b001111;
}

TEST_CASE("Register FF00") {
    Joypad jp(nullptr);

    SECTION("Default value is all keys released and nothing requested") {
        CHECK(jp.read_byte() == 0b00111111);
    }

    SECTION("Press a key when not requesting any key type") {
        jp.press_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00111111);
    }

    SECTION("Press a key when requesting the other key type") {
        jp.write_byte(REQUEST_DIRECTION_BUTTONS);
        jp.press_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00101111);
        jp.release_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00101111);
    }

    SECTION("Press/release a key when requesting the matching key type") {
        jp.write_byte(REQUEST_ACTION_BUTTONS);
        jp.press_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00011110);
        jp.release_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00011111);
    }

    SECTION("Requesting both key types while only action keys are pressed") {
        jp.write_byte(REQUEST_BOTH_TYPES);
        jp.press_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00001110);
        jp.release_key(Joypad::Keys::A);
        CHECK(jp.read_byte() == 0b00001111);
    }

    SECTION("Requesting both key types while only movement keys are pressed") {
        jp.write_byte(REQUEST_BOTH_TYPES);
        jp.press_key(Joypad::Keys::Left);
        CHECK(jp.read_byte() == 0b00001101);
        jp.release_key(Joypad::Keys::Left);
        CHECK(jp.read_byte() == 0b00001111);
    }
}