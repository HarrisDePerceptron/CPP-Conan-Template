#!/bin/bash
set -e

# Default build type
BUILD_TYPE="Release"

# Optional flags
TEST_FILTER=""
USE_CTEST=false
USE_COVERAGE=false

# Parse args
for arg in "$@"; do
  case $arg in
  --filter=*) TEST_FILTER="${arg#*=}" ;;
  --ctest) USE_CTEST=true ;;
  --coverage) USE_COVERAGE=true ;;
  --debug | -d) BUILD_TYPE="Debug" ;;
  --release | -r) BUILD_TYPE="Release" ;;
  --type)
    BUILD_TYPE="$2"
    shift
    ;;
  *)
    echo "❌ Unknown option: $arg"
    echo "Usage: $0 [--debug|--release|--type Debug|Release] [--filter=<TestNameOrTag>] [--ctest] [--coverage]"
    exit 1
    ;;
  esac
done

BUILD_DIR="build/$BUILD_TYPE"
TEST_BINARY="tests"
LCOV_REPORT_DIR="coverage-report/$BUILD_TYPE"

# Detect Conan
if ! command -v conan &>/dev/null; then
  echo "🔧 Conan not found. Installing via pip..."
  python3 -m pip install --upgrade pip
  python3 -m pip install conan
fi

echo "📦 Conan version: $(conan --version)"
conan profile detect --force

# Set up build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Install dependencies with build type
conan install ../.. --output-folder=. --build=missing -s build_type=$BUILD_TYPE -o "*:shared=False"

# Configure CMake
CMAKE_FLAGS="-DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE"

if [ "$USE_COVERAGE" = true ]; then
  echo "🛡️  Enabling code coverage instrumentation"
  CMAKE_FLAGS="$CMAKE_FLAGS -DENABLE_COVERAGE=ON"
fi

cmake ../.. $CMAKE_FLAGS

# Build test target
cmake --build . --target tests

# Run tests
echo -e "\n🚀 Running tests ($BUILD_TYPE)..."
if [ "$USE_CTEST" = true ]; then
  ctest --verbose
else
  if [ -n "$TEST_FILTER" ]; then
    "$TEST_BINARY" "$TEST_FILTER"
  else
    echo "current directory: $(pwd)"
    ./"$TEST_BINARY"
  fi
fi

# Generate coverage report
if [ "$USE_COVERAGE" = true ]; then
  echo -e "\n📊 Generating LCOV coverage report..."

  if ! command -v lcov &>/dev/null || ! command -v genhtml &>/dev/null; then
    echo "⚠️  lcov/genhtml not found. Please install: sudo apt install lcov"
    exit 1
  fi

  lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1
  lcov --remove coverage.info '/usr/*' '*/conan/*' '*/tests/*' -o filtered.info --rc lcov_branch_coverage=1

  rm -rf "$LCOV_REPORT_DIR"
  genhtml filtered.info --output-directory "$LCOV_REPORT_DIR" --branch-coverage

  echo "✅ Coverage report generated at: $BUILD_DIR/$LCOV_REPORT_DIR/index.html"
fi
