from conans import ConanFile
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain, CMake


class GameBoyEmulatorConan(ConanFile):
    name = "game_boy_emulator"
    settings = "os", "compiler", "build_type", "arch"
    license = "GPL-3.0-or-later"
    author = "Darius Arnold"
    version = "0.1.0"

    def requirements(self):
        self.requires("fmt/9.1.0")
        self.requires("imgui/1.88")
        self.requires("magic_enum/0.8.1")
        self.requires("spdlog/1.11.0")
        self.requires("argparse/2.9")
        self.requires("boost/1.80.0")
        self.requires("sdl/2.26.1")
        self.requires("catch2/2.13.6")
        if self.settings.os != "Emscripten":
            self.requires("nativefiledialog/116")
            # Overrides for nativefiledialogs gtk dependency which cant be built with recent compilers (clang/gcc)
            # since there are warnings as errors.
            self.requires("gtk/system", override=True)

    def export_sources(self):
        self.copy("*", excludes=("build*", "cmake-build*"))

    def layout(self):
        cmake_layout(self)

    def configure(self):
        self.options["boost"].header_only = True
        if self.settings.os != "Emscripten":
            self.options["sdl"].nas = False

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        # Workaround to be able to build the consumer (game_boy_emulator) with both Debug and Release while the
        # dependencies are built with whatever built type (most likeley Release) and still be able to use find_package
        # to find the dependencies.
        deps = CMakeDeps(self)
        deps.configuration = "Release"
        deps.generate()
        deps.configuration = "Debug"
        deps.generate()

    def build(self):
        cmake = CMake(self)
        # Since conan is mostly used for releases, we dont want sanitizers enabled and we dont want to run clang-tidy
        # during the build since it is slow (and was run in CI for the commit already).
        cmake.configure(variables={"SANITIZE": "OFF", "TIDY": "OFF"})
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def imports(self):
        # Copy the imgui bindings to the package
        self.copy("imgui_impl_sdl*", dst="bindings", src="res/bindings", root_package="imgui")

    def deploy(self):
        # Copy just the executable from the cache to the local filesystem
        self.copy("game_boy_emulator*", dst="", src="bin")
