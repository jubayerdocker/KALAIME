# Prebuilt GCC Binary Provider Investigation

## Comparison

| Method | GCC 16.2 exact | Prebuilt | Native Linux | Portable | Trust | Recommendation |
|---|---|---|---|---|---|---|
| Ubuntu 26.10 native APT | Exact 16.2.0 package name | Yes | Yes | System package | High | Use only on 26.10 |
| Conda-forge GCC packages | Exact 16.2.0 | Package artifacts verified | x86_64 linux-64 | Conda environment only; standalone not demonstrated | High community | CONDITIONAL research candidate |
| Linuxbrew/Homebrew GCC bottle | Exact 16.2.0 | Bottle record verified | x86_64 Linux bottle | Linuxbrew environment only; standalone not demonstrated | High community | Alternative after host testing |
| Ubuntu Toolchain Test PPA | Snapshot, not stable 16.2.0 | Yes | Native Ubuntu packages | Not user-prefix clean | Medium/unsupported | Reject as exact-version method |
| Official GCC source archive | Exact 16.2.0 | No | Source | Build-time portability | Highest source authenticity | Not a prebuilt solution |
| From-source bootstrap | Exact possible | No | Native after build | Configurable | High if reproducible | Reject for this requirement |

## Portable binary assessment

### A. Conda environment installation

Verified as package metadata: Conda can create a prefixed environment containing
the activation packages, implementation packages, binutils, runtime libraries
and sysroot. The activation scripts set compiler-related environment variables,
rpaths, CMake arguments and `CONDA_BUILD_SYSROOT`.

### B. Standalone extraction

Not demonstrated. No package archive was downloaded or extracted. The package
has `has_prefix=true` for implementation and sysroot packages, so relocation
cannot be assumed to work by copying extracted directories.

### C. Fully portable standalone toolchain

Not established. The required evidence is a real prefix extraction or relocation,
version output, `ldd`, dependency resolution, CMake build and runtime test on
both Ubuntu 22.04 and 24.04.

## Conda-forge

Evidence:

- [gcc_linux-64 package](https://anaconda.org/conda-forge/gcc_linux-64)
- [gcc_impl_linux-64 package](https://anaconda.org/conda-forge/gcc_impl_linux-64)
- [gxx_linux-64 package](https://anaconda.org/conda-forge/gxx_linux-64)
- [gxx_impl_linux-64 package](https://anaconda.org/conda-forge/gxx_impl_linux-64)
- [libstdcxx-ng package](https://anaconda.org/conda-forge/libstdcxx-ng)
- [ctng compiler activation feedstock](https://github.com/conda-forge/ctng-compiler-activation-feedstock)

The linux-64 implementation package depends on:

- binutils implementation;
- GCC 16.2 runtime libraries;
- libgcc/libgomp/libstdc++;
- sanitizer runtime;
- GCC 16.2 development libraries;
- a sysroot package.

Example latest linux-64 implementation artifact metadata:

- `gcc_impl_linux-64-16.2.0-h9891192_7.conda`
- SHA256: `14acaa661e4c4efc65606612b8b08f8f079c6a9e6da3ffcb77056685c675651`
- Native machine: `x86_64`
- Target triplet: `x86_64-any-linux`

The exact build should be pinned rather than accepting an older or newer build.

Latest verified Linux-64 artifacts at the time of audit:

| Package | Version/build | Size | SHA256 |
|---|---|---:|---|
| `gcc_linux-64` | `16.2.0-h0ab548f_4` | 29,937 bytes | `59c8811ac725d12882984b9f46961eeb6ad71eb46ee4827df3068fca614426d9` |
| `gcc_impl_linux-64` | `16.2.0-h9891192_7` | 86,085,260 bytes | `14acaa661e4c4ef6c65606612b8b08f8f079c6a9e6da3ffcb77056685c675651` |
| `gxx_linux-64` | `16.2.0-h223162a_4` | 28,339 bytes | `8c0bd8b59dbbc0e5c3acb62e80c5de774e094fb6051e0b97422736b73dec83f8` |
| `gxx_impl_linux-64` | `16.2.0-h0d273dc_7` | 16,987,505 bytes | `f873532a86ce9073a7255dec909472c2a271a57db938f383b630a3caa0b47709` |
| `libstdcxx-ng` | `16.2.0-hdf11a46_7` | 28,665 bytes | `337f481db36cc357884f05f404ccd1b3a7e0fc1ee48ec5c884e4281d213bc863` |
| `binutils_linux-64` | `2.46.1-default_h4852527_103` | 36,335 bytes | `606a42c754752fb905fe2012d52ff370d47b5b6adefd5f7befc4ec30ce758f23` |
| `sysroot_linux-64` | `2.39-hc4b9eeb_6` | 42,049,621 bytes | `7d235e4337f122f3439d0196e1c1c6e715a5b88fcbf24f944510186c84168e45` |

The table's sysroot line uses a verified release record. The exact package size
is 42,049,621 bytes. The package is `noarch`, but it enforces `__glibc >=2.39`.

## Linuxbrew/Homebrew

Evidence:

- [Homebrew GCC formula API](https://formulae.brew.sh/api/formula/gcc.json)
- Stable formula version: 16.2.0
- Linux x86_64 bottle SHA256:
  `6504ecae947061ffe79c9dea2f635791bf54b148d1063ec857873eac2a67d922`
- Linux ARM64 bottle SHA256:
  `a816cc94416b72b6b90b87db9479b12addbb1b877603ecfc7157345621a1e2e9`
- Formula source checksum:
  `e6738e29597f733270731aa90600f37ffdc045079dfc27ec7e8192cc81085c3e`

This is a repository-maintained bottle with executable names such as `gcc-16`
and `g++-16`. It remains unverified on Ubuntu 22.04/24.04 until the bottle is
executed on those distributions.

## Rejected providers

- Official GCC source mirrors: source only.
- Random GitHub GCC archives: no exact verified candidate was found.
- Ubuntu PPA: package is snapshot or development build, not stable 16.2.0 exact.
- Cross-compilers and embedded/ARM/RISC-V toolchains: not ordinary native Linux x86_64.
