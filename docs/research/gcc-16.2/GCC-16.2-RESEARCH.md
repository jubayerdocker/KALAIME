# GCC 16.2.0 Research

## Scope and conclusion

GCC 16.2.0 is a real, official GCC release. The GCC release page states it was
released on 2026-08-07 as a bug-fix release in the GCC 16 series.

Official GNU sources provide `gcc-16.2.0.tar.gz`, `gcc-16.2.0.tar.xz`,
diff archives and detached signatures. These are source archives, not native
prebuilt Ubuntu binaries.

The GCC binary-installation page does not list an official native Linux x86_64
GCC 16.2.0 binary package. It links third-party binary providers and explicitly
states that GCC did not create those binaries and does not support them.

## Audit status

Package metadata and channel records are verified. Binary execution, executable
paths, standalone extraction and Ubuntu compatibility are not verified.

## Exact Linux-64 package candidates

### Conda-forge

Conda-forge publishes native Linux x86_64 packages for GCC 16.2.0, including:

- `gcc_linux-64` 16.2.0 (activation scripts)
- `gcc_impl_linux-64` 16.2.0
- `gxx_linux-64` 16.2.0
- `gxx_impl_linux-64` 16.2.0
- `libstdcxx-ng` 16.2.0
- `binutils_linux-64`
- `sysroot_linux-64`

The packages are available from conda-forge's official Anaconda.org namespace
and expose SHA256/MD5 metadata through the Anaconda API.

This is a native Linux package set with verified package metadata, not an official
standalone GCC Project binary release and not an execution-verified portable
toolchain. It can be resolved into a Conda prefix under `$HOME/toolchains/`,
but that is Conda environment installation, not demonstrated standalone
portability. It does not require system alternatives based on package metadata,
but runtime coexistence remains untested.

### Linuxbrew/Homebrew

The Homebrew core formula for `gcc` reports:

- stable version: 16.2.0
- license: GPL-3.0-or-later with GCC exception
- native Linux x86_64 bottle
- source URL and source checksum
- bottle SHA256
- executable names including `gcc-16`, `g++-16`, `gcov-16`, and GCC binutils wrappers

This is a prebuilt Linux bottle when a compatible bottle is available. It is
not an official GCC binary release.

## Not verified as exact prebuilt native binaries

- Ubuntu 22.04 packages: no `gcc-16` in the base Noble/Jammy package search evidence.
- Ubuntu 24.04 packages: no `gcc-16` in Noble package search evidence.
- Ubuntu Toolchain Test PPA: published a March 2026 snapshot package named
  `16-20260315-1ubuntu1~24~ppa1`, not the 16.2.0 stable release.
- Ubuntu 26.04: package search found a GCC snapshot package
  `16-20260322-1ubuntu1`, not GCC upstream 16.2.0.
- Ubuntu 26.10: native package `gcc-16` is `16.2.0-3ubuntu1`, exact upstream
  compiler version in the package name. Compatibility is native to 26.10.

## Official sources

- [GCC 16.2 release announcement](https://gcc.gnu.org/gcc-16/)
- [GCC releases](https://gcc.gnu.org/releases.html)
- [Official source mirror directory](https://ftp.gnu.org/gnu/gcc/gcc-16.2.0/)
- [GCC precompiled-binaries page](https://gcc.gnu.org/install/binaries.html)
- [GCC 16.2 documentation](https://gcc.gnu.org/onlinedocs/16.2.0/)
- [GCC installation sources](https://gcc.gnu.org/install/)
