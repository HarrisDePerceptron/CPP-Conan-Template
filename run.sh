#!/bin/bash
set -e

./build.sh

BUILD_DIR="./build"
# Automatically extract project name (first occurrence in root CMakeLists.txt)
PROJECT_NAME=$(grep -m1 "^project(" CMakeLists.txt | sed -E 's/project\(([^ ]+).*/\1/')

if [ -z "$PROJECT_NAME" ]; then
  echo "Failed to detect project name from CMakeLists.txt"
  exit 1
fi

echo "🔧 Project binary: $PROJECT_NAME"

cd $BUILD_DIR
# Run the compiled binary (auto-detected)
echo -e "\n🚀 Running: $PROJECT_NAME"
./"$PROJECT_NAME"
