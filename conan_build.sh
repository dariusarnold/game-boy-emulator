#!/usr/bin/env bash

# Build project with conan.
# Dependencies (outside of those provided by dev container) are installed using conan.
# 
# Arguments:
# --package: Create a conan package instead of building locally. Defaults to false.
# 1: Conan profile path or name. Defaults to profiles/gcc-14.
# 2: Conan/CMake build type. Defaults to Release.


set -euo pipefail

PACKAGE_MODE=false

# Check for --package flag
if [[ "${1:-}" == "--package" ]]; then
  PACKAGE_MODE=true
  shift
fi

CONAN_PROFILE="${1:-profiles/gcc-14}"
BUILD_TYPE="${2:-Release}"

if [[ "$PACKAGE_MODE" == true ]]; then
  conan create . --profile:host="$CONAN_PROFILE" --profile:build="$CONAN_PROFILE" -s:a build_type="$BUILD_TYPE" --build=missing
else
  conan install . --output-folder build/conan_out --build=missing -pr:a "$CONAN_PROFILE" -s:a build_type="$BUILD_TYPE"
  conan build . --output-folder build/conan_out -pr:a "$CONAN_PROFILE" -s:a build_type="$BUILD_TYPE"
fi
