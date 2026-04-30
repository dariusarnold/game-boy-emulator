from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain, CMake
from conan.tools.files import copy
import pathlib


class GameBoyEmulatorConan(ConanFile):
    name = "game_boy_emulator"
    version = "0.1.0"
    package_type = "application"
    settings = "os", "compiler", "build_type", "arch"
    required_conan_version = ">=2.0"

    license = "GPL-3.0-or-later"
    author = "Darius Arnold"
    default_options = {
        "boost/*:header_only": True,
    }

    def requirements(self):
        self.requires("fmt/11.2.0")
        self.requires("imgui/1.88")
        self.requires("magic_enum/0.8.1")
        self.requires("spdlog/1.15.3")
        self.requires("argparse/2.9")
        self.requires("boost/1.80.0")

    def build_requirements(self):
        self.test_requires("catch2/2.13.10")

    def export_sources(self):
        for pattern in (
            "CMakeLists.txt",
            "COPYING",
            "cmake/*",
            "src/*",
        ):
            copy(self, pattern, src=self.recipe_folder, dst=self.export_sources_folder)

    def layout(self):
        cmake_layout(self)

    def generate(self):
        # Copy the imgui bindings to bindings directory for CMake build
        imgui = self.dependencies["imgui"]
        source_dir = pathlib.Path(imgui.package_folder) / "res" / "bindings"
        target_dir = pathlib.Path(self.source_folder) / "bindings"
        copy(self, pattern="imgui_impl_sdl*", src=source_dir, dst=target_dir)

        tc = CMakeToolchain(self)
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        # Since conan is mostly used for releases, we dont want sanitizers enabled and we dont want to run clang-tidy
        # during the build since it is slow (and was run in CI for the commit already).
        cmake.configure(variables={"SANITIZE": "OFF", "CLANG_TIDY": "OFF"})
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def deploy(self):
        # Copy just the executable from the cache to the local filesystem
        copy(self, "game_boy_emulator*", src=str(pathlib.Path(self.package_folder) / "bin"), dst=self.deploy_folder)
