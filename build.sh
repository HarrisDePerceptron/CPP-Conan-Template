#!/bin/bash
set -e

# Set up Conan build folder

BUILD_DIR="build"
DIST_DIR="dist"
mkdir -p "$BUILD_DIR" "$DIST_DIR"

PROJECT_NAME=$(grep -m1 "^project(" CMakeLists.txt | sed -E 's/project\(([^ ]+).*/\1/')

# Check if Conan is installed
if ! command -v conan &>/dev/null; then
  echo "Conan not found. Installing via pip..."
  python3 -m pip install --upgrade pip
  python3 -m pip install conan
else
  echo "Conan is already installed: $(conan --version)"
fi

# Detect compiler and platform settings
conan profile detect --force

# Install dependencies and generate toolchain in build dir
#conan install . --output-folder="$BUILD_DIR" --build=missing

conan install . \
  --output-folder="$BUILD_DIR" \
  --build=missing \
  -s build_type=Release \
  -o "*:shared=False"

# Run CMake with correct toolchain file
cd "$BUILD_DIR"

cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON #-DCMAKE_EXE_LINKER_FLAGS="-static" \

cmake --build .

cp "$PROJECT_NAME" "../$DIST_DIR/"
