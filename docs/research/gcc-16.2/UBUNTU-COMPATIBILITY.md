# Ubuntu Compatibility Matrix

| Ubuntu release | gcc-16 availability | Exact GCC 16.2.0 evidence | Recommended status |
|---|---|---|---|
| 22.04 Jammy | No official base package match found | No | Use a separately pinned prebuilt environment or do not claim support |
| 24.04 Noble | No official base package match found | No | Use Conda-forge or Linuxbrew only after compatibility verification |
| 26.04 Resolute | `gcc-16` snapshot package `16-20260322-1ubuntu1` | Package is a pre-release snapshot name, not confirmed upstream stable 16.2.0 | Upgrade/verify exact upstream version, do not assume exact |
| 26.10 Stonking | Native `gcc-16` package `16.2.0-3ubuntu1` | Exact upstream version in package name | Native APT is possible on 26.10 |

## Package availability

Ubuntu package search is primary evidence for each release:

- [Jammy search](https://packages.ubuntu.com/search?suite=jammy&searchon=names&keywords=gcc-16)
- [Noble search](https://packages.ubuntu.com/search?suite=noble&searchon=names&keywords=gcc-16)
- [Resolute gcc-16](https://packages.ubuntu.com/resolute/gcc-16)
- [Resolute g++-16](https://packages.ubuntu.com/resolute/g++-16)
- [Stonking gcc-16](https://packages.ubuntu.com/stonking/gcc-16)
- [Stonking g++-16](https://packages.ubuntu.com/stonking/g++-16)

## Runtime considerations

The package or prebuilt-toolchain runtime must be verified for:

- glibc compatibility;
- libgcc_s and libstdc++ resolution;
- binutils version;
- kernel headers/sysroot;
- dynamic loader;
- C++ ABI behavior;
- CMake and Ninja.

No binary compatibility claim is made for Ubuntu 22.04 or 24.04 until executed
on that exact distribution. A toolchain may compile against 24.04 headers even
when the package's glibc or libstdc++ runtime differs.

Local Ubuntu 24.04 evidence at audit time showed:

- glibc 2.39;
- Ubuntu GCC 13.3.0;
- Ubuntu libstdc++ 14.2.0;
- Ubuntu binutils 2.42.

This matches the host glibc family but does not prove the Conda GCC package
executes or produces ABI-compatible CMAKE/ICU/SQLite/GoogleTest dependencies.
