# GCC 16.2.0 CMake Integration

## CMake discovery

The Conda activation scripts are evidenced to set CMake compiler-related
arguments, sysroot and prefix paths. The actual executable names and package
contents are not verified without creating and activating an environment.
No KALAIME build or CMake project was executed for this audit.

Do not set `CMAKE_CXX_COMPILER` globally. Use an environment-specific CMake
configure command or a reviewed preset.

After activation, configure a separate build directory:

```bash
cmake -S . \
  -B build/gcc16 \
  -G Ninja \
  -DCMAKE_C_COMPILER="$PREFIX/bin/x86_64-conda-linux-gnu-gcc" \
  -DCMAKE_CXX_COMPILER="$PREFIX/bin/x86_64-conda-linux-gnu-g++" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON
```

The exact executable names must be discovered and verified in the selected
prefix before this command is used:

```bash
find "$PREFIX/bin" -maxdepth 1 -type f -o -type l |
  sort |
  grep -E '(gcc|g\+\+)'
```

Do not assume the Conda executable names until the package is installed and
tested. A Linuxbrew environment may use `gcc-16` and `g++-16` instead.

## Minimal C++20 verification

Create a temporary file outside the repository:

```cpp
#include <concepts>
#include <format>
#include <iostream>
#include <string>
#include <thread>

template <std::integral T>
T plus_one(T value) {
    return value + 1;
}

int main() {
    std::cout << plus_one(1) << '\n';
    std::cout << std::format("{} threads\n",
                             std::thread::hardware_concurrency());
}
```

Verify:

```bash
"$CXX" -std=c++20 -pthread "$SRC" -o gcc16-cpp20
./gcc16-cpp20
"$CXX" -dumpfullversion
ldd ./gcc16-cpp20
```

Run separate Debug and Release builds before changing the repository preset.
No production CMake file should be modified without explicit implementation
approval.

## Dependency and ABI status

No conflict with ICU, SQLite, GoogleTest or pthreads is claimed without a build.
If vcpkg is used, dependencies should be rebuilt or resolved with the selected
GCC triplet rather than mixing binaries from Ubuntu GCC 13.3.0 and GCC 16.2.0.
The resulting libstdc++/libgcc runtime must be checked with `ldd`.
