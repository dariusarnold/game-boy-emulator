# Building

## Dependencies

Conan 1.X is used for dependency management. The latest 1.Xversion is recommended (1.59.0 currently). The integration 
with CMake is transparent (find_package), so other approaches will work (such as installing the dependencies manually or 
from the system package manager).

## Local development

It is recommended to use Conan to install the dependencies. Using the profiles from 
https://github.com/dariusarnold/conan-profiles with `conan config install 
https://github.com/dariusarnold/conan-profiles.git`
makes them available for use with conan.
After the config install, the dependencies can be installed with `conan install . --build=missing -pr:b=gcc-12 -pr:h=gcc-12 -if build`.
Swapping gcc-12 for another profile name, will use a different compiler to build the dependencies.
Following that, a CMakeUserPresets.json should have been created and can be used to build the software with CMake. 
The commands for that can be found in the [ci.yml](.github/workflows/ci.yml) file.

### Integration into CLion

While the generated CMakeUserPresets.json file can be used directly in CLion, the build configurations generated by it
can't be modified, meaning the build type can't be changed. It is recommended to use the Conan generated toolchain file 
instead. This enables the normal mechanism of CLion to change the build type and other settings. If my profiles are 
used, the toolchain file will tell CMake where the compiler is located. Otherwise, a Toolchain (this time in CLion)
which sets the compiler has to be created.
Conan helpfully prints the cmake invocation required to use the generated toolchain file after the install command. 