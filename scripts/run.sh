#!/bin/bash
set -e

# Default to Release
BUILD_TYPE="Debug"

# Parse command-line args
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

./scripts/build.sh --type "$BUILD_TYPE"

BUILD_DIR="./build/$BUILD_TYPE"
PROJECT_NAME=$(grep -m1 "^project(" CMakeLists.txt | sed -E 's/project\(([^ ]+).*/\1/')

if [ -z "$PROJECT_NAME" ]; then
  echo "Failed to detect project name from CMakeLists.txt"
  exit 1
fi

echo "Build Type: $BUILD_TYPE"
echo "Project binary: $PROJECT_NAME"

cd "$BUILD_DIR"

# Run the compiled binary
echo -e "\n🚀 Running: $PROJECT_NAME"
./"$PROJECT_NAME"
