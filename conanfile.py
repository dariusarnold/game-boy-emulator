from conans import ConanFile
from conan.tools.cmake import cmake_layout


class GameBoyEmulatorConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("fmt/9.1.0")
        self.requires("imgui/1.88")
        self.requires("magic_enum/0.8.1")
        self.requires("spdlog/1.11.0")
        self.requires("argparse/2.9")
        self.requires("boost/1.80.0")
        self.requires("sdl/2.26.1")
        self.requires("catch2/2.13.6")
        self.requires("nativefiledialog/116")
        # Overrides for nativefiledialogs gtk dependency which cant be built with recent compilers (clang/gcc)
        # since there are warnins as errors.
        self.requires("gtk/system", override=True)

    def layout(self):
        cmake_layout(self)

    def configure(self):
        self.options["boost"].header_only = True
        self.options["sdl"].nas = False

    def imports(self):
        self.copy("imgui_impl_sdl*", dst="bindings", src="res/bindings", root_package="imgui")
