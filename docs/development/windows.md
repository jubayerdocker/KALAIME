# Windows Build and Verification

The only production platform target in this phase is Windows 10/11 x64. The
approved compiler is MSVC `cl.exe` from Visual Studio 2022's v143 toolset,
along with the Windows SDK, MSBuild, CMake, and vcpkg. MinGW, GCC, MSYS2,
clang-cl, and Zig are not supported Windows production compilers.

## Hosted build

`.github/workflows/windows-ci.yml` uses the `windows-2022` GitHub-hosted runner.
It verifies Visual Studio's x64 C++ tools, a v143 toolset directory, Windows
SDK headers, and `cl.exe /Bv`; then it configures
`Visual Studio 17 2022`, x64, and v143 via the `windows-msvc` preset.

The vcpkg root is created on the runner at the exact manifest baseline.
Configuration uses vcpkg manifest mode and the `testing` feature. CMake builds
Release and Debug, enables `/W4`, `/WX`, `/analyze`, and runs non-interactive
CTest tests. The workflow uploads DLL and executable outputs only after all
build and test steps pass.

## Local commands on an appropriately provisioned Windows host

From a Visual Studio 2022 developer environment with v143 and a Windows SDK:

```powershell
$env:VCPKG_ROOT = "C:\path\to\vcpkg-at-the-pinned-baseline"
cmake --preset windows-msvc
cmake --build --preset windows-msvc-release --parallel 1
ctest --preset windows-msvc-release
cmake --build --preset windows-msvc-debug --parallel 1
ctest --preset windows-msvc-debug
```

The CMake target deliberately errors unless the selected compiler uses v143.
Visual Studio's generator is multi-configuration; use the Release and Debug
build/test presets rather than setting `CMAKE_BUILD_TYPE`. The generator does
not emit `compile_commands.json`; Ubuntu clangd is for portable sources.

## What hosted CI does not validate

The native shared-library target is a build boundary with a trivial exported
smoke entry point. The source checks that a Windows SDK TSF interface
declaration is available and calls a Win32 API; no COM class factory, TSF
registration, or keyboard service is implemented. Hosted tests do not type
into desktop applications and do not
validate actual text composition, TSF activation, installer behavior, or
interactive Windows UI. Those require a controlled, interactive Windows
verification session.

Artifacts are attached to successful `Windows MSVC CI` workflow runs under
`suborno-windows-msvc-v143-x64`.
