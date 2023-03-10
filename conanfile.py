from conans import ConanFile, CMake

class GameBoyEmulatorConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "fmt/9.1.0", "catch2/2.13.6", "imgui/1.88", "magic_enum/0.8.1", "spdlog/1.11.0", "argparse/2.9", "boost/1.80.0", "sdl/2.26.1", ("nas/1.9.4", "override")
    generators = "cmake_find_package", "cmake_paths"

    def configure(self):
        self.options["boost"].header_only = True

    def imports(self):
        self.copy("imgui_imp_sdl*", dst="../bindings", src="res/bindings")
