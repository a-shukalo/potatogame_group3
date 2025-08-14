#!/usr/bin/env bash
set -euo pipefail

# Ensure we run from the repo root (script's directory)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Simple macOS build helper for CMake + Ninja + vcpkg
# Defaults:
#  - Detect host arch (arm64/x86_64) and build natively
#  - macOS deployment target 12.0
#  - Ninja generator
#  - Debug config (override with --config Release)

usage() {
  cat <<'USAGE'
Usage: ./build-macos.sh [--arch arm64|x86_64|universal] [--config Debug|Release] [--vcpkg-root PATH] [--clean]

Options:
  --arch         Target arch. Default: host arch. Use 'universal' for arm64;x86_64
  --config       CMake build type. Default: Debug
  --vcpkg-root   Path to vcpkg root (directory containing 'vcpkg' executable)
  --clean        Remove build directory before configure

Examples:
  ./build-macos.sh                                     # native debug build
  ./build-macos.sh --config Release                    # native release build
  ./build-macos.sh --arch universal --config Release   # universal release build
  ./build-macos.sh --vcpkg-root "$HOME/vcpkg"          # custom vcpkg path
USAGE
}

ARCH=""
CONFIG="Debug"
VCPKG_ROOT="${VCPKG_ROOT:-}"
CLEAN=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    -h|--help)
      usage; exit 0;;
    --arch)
      ARCH="${2:-}"; shift 2;;
    --config)
      CONFIG="${2:-}"; shift 2;;
    --vcpkg-root)
      VCPKG_ROOT="${2:-}"; shift 2;;
    --clean)
      CLEAN=1; shift;;
    *)
      echo "Unknown option: $1" >&2; usage; exit 1;;
  esac
done

# Detect host arch if not provided
if [[ -z "$ARCH" ]]; then
  HOST_ARCH=$(uname -m)
  case "$HOST_ARCH" in
    arm64) ARCH="arm64" ;;
    x86_64) ARCH="x86_64" ;;
    *) echo "Unsupported host arch: $HOST_ARCH" >&2; exit 1;;
  esac
fi

# Resolve vcpkg root
if [[ -z "$VCPKG_ROOT" ]]; then
  if [[ -d "$HOME/vcpkg" ]]; then
    VCPKG_ROOT="$HOME/vcpkg"
  elif [[ -d "./vcpkg" ]]; then
    VCPKG_ROOT="$(pwd)/vcpkg"
  else
    echo "vcpkg not found. Set VCPKG_ROOT env or pass --vcpkg-root PATH" >&2
    echo "Example: git clone https://github.com/microsoft/vcpkg.git $HOME/vcpkg && $HOME/vcpkg/bootstrap-vcpkg.sh" >&2
    exit 1
  fi
fi

TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
if [[ ! -f "$TOOLCHAIN_FILE" ]]; then
  echo "Invalid VCPKG_ROOT (missing scripts/buildsystems/vcpkg.cmake): $VCPKG_ROOT" >&2
  exit 1
fi

# Require Ninja
if ! command -v ninja >/dev/null 2>&1; then
  echo "Ninja is required. Install via: brew install ninja" >&2
  exit 1
fi

# Compose CMake arguments
OSX_DEPLOYMENT_TARGET="12.0"
if [[ "$ARCH" == "universal" ]]; then
  CMAKE_OSX_ARCHS="arm64;x86_64"
  BUILD_DIR="build/macos-universal-ninja"
else
  CMAKE_OSX_ARCHS="$ARCH"
  BUILD_DIR="build/macos-${ARCH}-ninja"
fi

if [[ $CLEAN -eq 1 && -d "$BUILD_DIR" ]]; then
  rm -rf "$BUILD_DIR"
fi
mkdir -p "$BUILD_DIR"
pushd "$BUILD_DIR" >/dev/null

echo "Configuring (arch=$CMAKE_OSX_ARCHS, config=$CONFIG) ..."
cmake -G Ninja \
  -DCMAKE_BUILD_TYPE="$CONFIG" \
  -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET="$OSX_DEPLOYMENT_TARGET" \
  -DCMAKE_OSX_ARCHITECTURES="$CMAKE_OSX_ARCHS" \
  ../..

echo "Building ..."
cmake --build . -- -j "$(sysctl -n hw.ncpu)"

echo
echo "Done. Executable is in: $BUILD_DIR"
echo "To run: $(pwd)/BrotatoGame"

popd >/dev/null


