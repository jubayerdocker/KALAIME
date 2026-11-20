# SUBORNO Verification Report

## Executed

- Ubuntu vcpkg configure/build/CTest passed.
- Unit test: passed.
- Integration test: passed.
- GCC 13 C++20 and existing engine probe passed.
- Portable Engine, layout registry and layout switching passed.
- Exact SQLite candidate integration passed.
- Formatting passed after clang-format-23.

## Repository state

- `main` branch.
- Existing AI ecosystem files are present.
- Existing source/CMake/CI files remain unchanged.

## Pending evidence

- Ubuntu configure/build/CTest.
- Clang-format.
- clang-tidy.
- Windows Debug/Release.
- Interactive TSF/COM registration and typing.
- Ubuntu configure/build/CTest/format/tidy: pass.
- ASan/UBSan CTest: pass (2/2 suites).
- libFuzzer: pass 1,000 runs in 0 seconds with no crash or sanitizer finding.
- Ubuntu CI Clang preset: 2/2 CTest suites passed.
- Dictionary corrupt/migration/prefix/learning tests: pass in CTest.
- ASan/UBSan rerun: 2/2 CTest suites passed.
- SBOM/Grype: no vulnerabilities found for gtest, ICU, SQLite and vcpkg helper packages.
- Clang 23.1.5 Release benchmark (bounded): transliteration 4.2 µs; candidate generation 37.1 µs.
