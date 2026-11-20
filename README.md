# SUBORNO Input (SUBORNO-IME)

**SUBORNO** is the brand; **SUBORNO Input** is the product; **SUBORNO-IME** is the
technical project.

SUBORNO-IME is a portable C++20 Bangla phonetic engine under active development,
with a native Windows TSF/COM adapter planned. The repository now contains
a working phonetic parser, layout registry, independent composition sessions,
SQLite-backed dictionary, candidate ranking, settings persistence and build
infrastructure. Native Windows typing and COM/TSF registration are not complete.

## Scope and platform

- Production target: Windows 10/11 x64.
- Windows integration direction: native MSVC v143, Win32, COM, and Text Services
  Framework (TSF).
- Ubuntu 24.04 is the primary development environment for the platform-neutral
  C++20 code.
- The source and target boundaries keep the portable core separate from the
  Windows adapter. Linux, macOS, and Android input method adapters are not
  implemented or claimed as supported.
- The current Windows target is a DLL build boundary, not a registered or
  functioning TSF text service.

See [the architecture overview](docs/architecture/overview.md) and
[development setup](docs/development/ubuntu.md) for details.

## Technology

- C++20 and CMake with CMake Presets
- Ubuntu: Clang 18, Ninja, clangd, clang-format, and clang-tidy
- Windows production build: MSVC v143, Windows SDK, MSBuild, and the Visual
  Studio 17 2022 CMake generator
- vcpkg manifest mode with a pinned registry baseline
- ICU4C and SQLite as production dependencies; WIL on Windows only
- GoogleTest and Google Benchmark as optional development dependencies

## Prerequisites

Ubuntu 24.04 x86_64, Git, and `sudo` for installing missing system packages.
Run `scripts/bootstrap-ubuntu.sh` to detect and install missing tools and set
up the pinned vcpkg checkout. Windows compilation is performed by the
`windows-2022` GitHub Actions runner; Ubuntu is not expected to provide a
Windows SDK or MSVC.

## Build and test on Ubuntu

```bash
scripts/bootstrap-ubuntu.sh
export PATH="$HOME/.local/bin:$PATH"
export VCPKG_ROOT="$HOME/.cache/suborno/vcpkg"
scripts/configure-ubuntu.sh
scripts/build-ubuntu.sh
scripts/test-ubuntu.sh
scripts/check-format-ubuntu.sh
scripts/check-tidy-ubuntu.sh
```

By default, builds use one job. Set `SUBORNO_JOBS` to a positive integer to
override this for `scripts/build-ubuntu.sh`. Benchmarks are disabled in normal
development builds; see [Ubuntu development instructions](docs/development/ubuntu.md)
to enable the optional target.

## Windows build

The Windows CI workflow verifies MSVC v143 and the Windows SDK, configures the
`windows-msvc` preset, builds x64 Release and Debug, and runs non-interactive
tests. It produces a native Windows DLL build boundary only; it is not a
registered or functioning TSF service. See
[Windows build instructions](docs/development/windows.md).

## Tests

GoogleTest infrastructure tests verify that the portable core library builds,
links, and exposes its initial infrastructure ABI marker. They do not test
input method or Bangla-language behavior. CTest is used on Ubuntu and Windows.
The optional benchmark is only a build/configuration smoke target and does not
represent IME performance.

## Security and dependencies

Dependency versions, licenses, and source URLs are listed in
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). Security reporting is
described in [SECURITY.md](SECURITY.md). No signing certificates or credentials
belong in this repository.
