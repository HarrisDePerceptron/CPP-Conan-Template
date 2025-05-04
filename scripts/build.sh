#!/bin/bash
set -e

# Default to Release
BUILD_TYPE="Release"

# Parse command line arguments
while [[ "$#" -gt 0 ]]; do
  case $1 in
  --debug | -d) BUILD_TYPE="Debug" ;;
  --release | -r) BUILD_TYPE="Release" ;;
  --type)
    BUILD_TYPE="$2"
    shift
    ;;
  *)
    echo "Unknown parameter: $1"
    exit 1
    ;;
  esac
  shift
done

BUILD_DIR="build/$BUILD_TYPE"
DIST_DIR="dist/$BUILD_TYPE"
TEMPLATE_DIR="templates"

mkdir -p "$BUILD_DIR" "$DIST_DIR"

# Extract project name from CMakeLists.txt
PROJECT_NAME=$(grep -m1 "^project(" CMakeLists.txt | sed -E 's/project\(([^ ]+).*/\1/')

if [ -z "$PROJECT_NAME" ]; then
  echo "❌ Failed to detect project name from CMakeLists.txt"
  exit 1
fi

# Check Conan
if ! command -v conan &>/dev/null; then
  echo "Conan not found. Installing via pip..."
  python3 -m pip install --upgrade pip
  python3 -m pip install conan
else
  echo "Conan is already installed: $(conan --version)"
fi

# Detect compiler/platform
conan profile detect --force

# Install dependencies
conan install . \
  --output-folder="$BUILD_DIR" \
  --build=missing \
  -s build_type=$BUILD_TYPE \
  -o "*:shared=False"

# Configure + build
cd "$BUILD_DIR"

cmake ../.. \
  -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
  -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build .

# Copy the binary and templates to dist/BuildType/
cp "$PROJECT_NAME" "../../$DIST_DIR/"
cp -r "../../$TEMPLATE_DIR" ./
cp -r "../../$TEMPLATE_DIR" "../../$DIST_DIR/"
