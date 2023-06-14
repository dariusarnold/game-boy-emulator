# game-boy-emulator

A game boy emulator written in C++ and built with Conan and CMake.

![Example Screenshot showing Pokemon Red](images/screenshot01.png?raw=true)

## Features

- Debug drawing of sprites, background, window and VRAM
- Fast-forward
- Supports battery backed saves
- Line based renderer
- Partial audio emulation (currently channel 1 and 2)
- NoMBC, MBC1, MBC3, MBC5 supported (No RTC support)
- Passing all of blarggs test roms
- Passes the DMG acid2 graphics test rom
- Performance graph

### Technical

- Compiled with warnings as errors (and a large selection of warnings enabled, including Weverything on clang)
- Tested with UBSanitizer, LeakSanitizer and AddressSanitizer
- Required clang-tidy checks
- Dependency management using Conan
  - Currently conan 1.X is supported (version 1.59.0 at time of writing)
- Builds on Linux (clang, gcc) and Windows (Mingw)
- Github Actions based pipeline (builds with clang, gcc and runs the tests)

### Building

Install the dependencies using conan (please modify build and host profile to suit your configuration):
```
conan install -pr:b default -pr:h default .
```
Alternatively use a package manager to install them system wide.

Show the preset created by conan:
```
cmake --list-presets
```

Generate the build system using the CMake preset created by conan (use the name from the previous command):
```
cmake --preset linux-x86_64-gcc-12-release
```

Build the software:
```
cmake --build --preset linux-x86_64-gcc-12-release
```

Install from the build directory (Please modify the path to suit your system):
```
cmake --install --prefix out build/linux-x86_64-gcc-12-release
```

## Controls

| Keyboard | Gameboy Key  |
|----|---|
| J | A |
| K  | B |
| N  | Start |
| L  | Select |
| WASD | D-Pad |
