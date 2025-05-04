#!/bin/bash
set -e

BUILD_DIR="build"
TEST_BINARY="./tests"
LCOV_REPORT_DIR="coverage-report"

# Optional flags
TEST_FILTER=""
USE_CTEST=false
USE_COVERAGE=false

# Parse arguments
for arg in "$@"; do
  case $arg in
  --filter=*)
    TEST_FILTER="${arg#*=}"
    ;;
  --ctest)
    USE_CTEST=true
    ;;
  --coverage)
    USE_COVERAGE=true
    ;;
  *)
    echo "❌ Unknown option: $arg"
    echo "Usage: $0 [--filter=<TestNameOrTag>] [--ctest] [--coverage]"
    exit 1
    ;;
  esac
done

# Check for Conan
if ! command -v conan &>/dev/null; then
  echo "🔧 Conan not found. Installing via pip..."
  python3 -m pip install --upgrade pip
  python3 -m pip install conan
fi

echo "📦 Conan version: $(conan --version)"
conan profile detect --force

# Set up build directory
mkdir -p "$BUILD_DIR"

# Conan install
conan install . --output-folder="$BUILD_DIR" --build=missing

# Configure CMake with coverage flag if needed
cd "$BUILD_DIR"
CMAKE_FLAGS="-DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake"
if [ "$USE_COVERAGE" = true ]; then
  echo "🛡️  Enabling code coverage instrumentation"
  CMAKE_FLAGS="$CMAKE_FLAGS -DENABLE_COVERAGE=ON"
fi

cmake .. $CMAKE_FLAGS

# Build only test target
cmake --build . --target tests

# Run tests
echo -e "\n🚀 Running tests..."
if [ "$USE_CTEST" = true ]; then
  ctest --verbose
else
  if [ -n "$TEST_FILTER" ]; then
    $TEST_BINARY "$TEST_FILTER"
  else
    $TEST_BINARY
  fi
fi

# Generate coverage report
if [ "$USE_COVERAGE" = true ]; then
  echo -e "\n📊 Generating LCOV coverage report..."

  if ! command -v lcov &>/dev/null || ! command -v genhtml &>/dev/null; then
    echo "⚠️  lcov/genhtml not found. Please install: sudo apt install lcov"
    exit 1
  fi

  # Collect coverage data
  lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1
  lcov --remove coverage.info '/usr/*' '*/conan/*' '*/tests/*' -o filtered.info --rc lcov_branch_coverage=1

  # Generate HTML report
  rm -rf "$LCOV_REPORT_DIR"
  genhtml filtered.info --output-directory "$LCOV_REPORT_DIR" --branch-coverage

  echo "✅ Coverage report generated at: $BUILD_DIR/$LCOV_REPORT_DIR/index.html"
fi
