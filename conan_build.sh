#!/usr/bin/env bash

# Build project with conan.
# Dependencies (outside of those provided by dev container) are installed using conan.
# 
# Arguments:
# 1: Conan profile path or name. Defaults to profiles/gcc-14.
# 2: Conan/CMake build type. Defaults to Release.


set -euo pipefail

CONAN_PROFILE="${1:-profiles/gcc-14}"
BUILD_TYPE="${2:-Release}"

conan install . --output-folder build/conan_out --build=missing -pr:a "$CONAN_PROFILE" -s:a build_type="$BUILD_TYPE"
conan build . --output-folder build/conan_out -pr:a "$CONAN_PROFILE" -s:a build_type="$BUILD_TYPE"
