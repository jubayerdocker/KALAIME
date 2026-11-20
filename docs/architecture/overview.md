# Architecture and Build Boundaries

## Current status

The repository contains a portable Bangla phonetic prototype, layout registry,
composition sessions, settings persistence and SQLite dictionary/candidate
integration. Native Windows COM/TSF registration and desktop typing are not yet
implemented.

## Platform boundaries

- `kalaime_core` is a portable C++20 static library.
- `kalaime_unicode` carries ICU link requirements; `kalaime_persistence`
  carries SQLite requirements. `kalaime_layout_api` and
  `kalaime_dictionary` mark intended module boundaries but implement no
  product feature.
- `src/platform/windows/` is excluded from non-Windows configurations.
  `kalaime_windows_tsf` is a native shared-library target and build boundary;
  its smoke source includes the Windows SDK TSF interface declarations and
  calls a Win32 API to verify that the DLL target compiles and links. It is not
  a COM server or TSF text service.
- WIL is only found and linked for the Windows target. Ubuntu portable targets
  cannot include Windows SDK, COM, TSF, or WIL headers.
- Windows production configurations fail unless CMake selects MSVC toolset
  v143. No MinGW, GCC, clang-cl, or Zig fallback is permitted.

Future platform adapters should be separate targets that depend on the
platform-neutral API, not on Windows implementation details. Linux, macOS, and
Android adapters are outside the current target.

## Build and dependency graph

```text
kalaime_core
  ├── kalaime_unicode ── ICU4C
  ├── kalaime_layout_api
  ├── kalaime_dictionary
  └── kalaime_persistence ── SQLite

kalaime_windows_tsf (Windows only, shared library)
  └── WIL

kalaime_unit_tests / kalaime_integration_tests
  └── GoogleTest (development feature)

kalaime_benchmarks (optional)
  └── Google Benchmark (development feature)
```

`BUILD_TESTING` controls GoogleTest targets. `KALAIME_BUILD_BENCHMARKS` is off
by default. CMake target-scoped options enforce C++20 and warning policies.
Ubuntu presets use Ninja and export `compile_commands.json`; Visual Studio
generators use their supported configuration mechanism and do not rely on that
database.

## Verification boundaries

Ubuntu CI exercises the portable core and tests with Clang and GCC. Windows CI
builds the x64 DLL and tests using MSVC v143 and the Windows SDK. Hosted
Windows checks are compile and non-interactive tests; COM/TSF registration and
actual desktop typing need a separate interactive Windows test environment.
