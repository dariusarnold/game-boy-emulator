#!/usr/bin/env bash

# Build project with conan.
# Dependencies (outside of those provided by dev container) are installed using conan.
# 
# Arguments:
# 1: Conan Profile: Name of conan profile to use.


set -euo pipefail

CONAN_PROFILE="gcc-14"
if [ $# -gt 0 ]; then
  CONAN_PROFILE="$1"
fi

conan install . --output-folder build --build=missing -c tools.cmake.cmaketoolchain:generator=Ninja -pr:a "$CONAN_PROFILE"
conan build . -pr:a "$CONAN_PROFILE"
