# Requirements

- Linux (primary development environment)
- C++17-compatible compiler (GCC 8+ or Clang 7+)
- CMake 3.16+
- Make or Ninja
- Git (if cloning the repository)

On Ubuntu, install the build tools with:

```
sudo apt update
sudo apt install build-essential cmake git
```

The current implementation uses the C++ standard library. PostgreSQL is not required to build or run the current tests.

# Build and Run

Run all commands from the project root (the directory containing `CMakeLists.txt`).

## 1. Configure the project

```
cmake -S . -B build
```

## 2. Build

```
cmake --build build -j
```

## 3. Run all tests

```
ctest --test-dir build --output-on-failure
```

For verbose test output:

```
ctest --test-dir build --verbose
```

To list registered tests:

```
ctest --test-dir build -N
```

# Run Tests with AddressSanitizer and UndefinedBehaviorSanitizer

Configure a separate sanitizer build:

```
cmake -S . -B build-asan \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer -g" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
```

Build and run:

```
cmake --build build-asan -j
ctest --test-dir build-asan --output-on-failure
```

# Clean and Rebuild

To remove generated build files:

```
rm -rf build build-asan
```

Then configure, build, and test again:

```
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
```
