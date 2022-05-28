#include "constants.h"
#include "emulator.hpp"
#include "io.hpp"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include <filesystem>
#include <fstream>
#include <optional>
#include "SFML/Graphics.hpp"
#include "bitmanipulation.hpp"


template <typename Container>
void print_container_hex(const Container& c) {
    fmt::print("[{:02X}]\n", fmt::join(c, ", "));
}


const sf::Color color0(255, 255, 255, 255);
const sf::Color color1(200, 200,200, 255);
const sf::Color color2(100, 100, 100, 255);
const sf::Color color3(0, 0, 0, 255);




int main() {
    /*
    sf::RenderWindow window(sf::VideoMode(160, 144), "Emulator");

    std::vector<uint8_t> frame(160*144);
    std::vector tile = {0xFF, 0x00, 0x7E, 0xFF, 0x85, 0x81, 0x89, 0x83, 0x93, 0x85, 0xA5, 0x8B, 0xC9, 0x97, 0x7E, 0xFF};
    int i = 0;
    for (auto& x: frame) {
        x = tile[i % tile.size()];
        ++i;
    }

    sf::Clock clock;
    sf::Time last_time = clock.getElapsedTime();
    while (window.isOpen()) {
        sf::Time current_time = clock.getElapsedTime();
        float fps = 1.0f / (current_time.asSeconds() - last_time.asSeconds());
        last_time = current_time;
        fmt::print("FPS: {}\n", fps);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear(sf::Color::Black);

        std::vector<sf::Uint32> pixel_data(160 * 144);
        int i = 0;
        for (auto& x : pixel_data) {
            x = color1.toInteger();
            ++i;
        }
        pixel_data[10] = color3.toInteger();
        sf::Image image;
        image.create(160, 144, reinterpret_cast<const sf::Uint8*>(&pixel_data[0]));
        sf::Texture texture;
        texture.loadFromImage(image);
        sf::Sprite sprite;
        sprite.setTexture(texture);
        window.draw(sprite);

        window.display();
    }
    return 0;
     */


    auto boot_rom_path = std::filesystem::absolute(("../../dmg01-boot.bin"));
    auto boot_rom = load_boot_rom_file(boot_rom_path);
    if (!boot_rom) {
        return -1;
    }
    auto rom_path = std::filesystem::absolute("../../PokemonRed.gb");
    auto game_rom = load_rom_file(rom_path);
    if (game_rom.empty()) {
        return -1;
    }

    print_container_hex(boot_rom.value());
    Emulator emulator(boot_rom.value(), game_rom);
    emulator.run();
    return 0;
}
