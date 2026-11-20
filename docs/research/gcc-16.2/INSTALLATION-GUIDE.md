# GCC 16.2.0 Installation Guide

This document describes only verified package sources. This research task does
not execute an installer or download an executable.

## Option 1: Ubuntu 26.10 native package

On Ubuntu 26.10 (`stonking`) only:

```bash
sudo apt-get update
sudo apt-get install gcc-16 g++-16 libstdc++-16-dev binutils
g++-16 -dumpfullversion
```

These package names and versions are verified by packages.ubuntu.com. Do not
assume these packages exist on Ubuntu 22.04 or 24.04.

Do not run `update-alternatives`; KALAIME should coexist with the existing
Ubuntu GCC.

## Option 2: Conda-forge exact GCC 16.2.0 prefix

Use this only after an existing trusted Conda/Miniforge installation is
verified and its installer checksum is known.

```bash
PREFIX="$HOME/toolchains/gcc-16.2.0"

conda create \
  --prefix "$PREFIX" \
  --channel conda-forge \
  --strict-channel-priority \
  gcc_linux-64=16.2.0=h0ab548f_4 \
  gcc_impl_linux-64=16.2.0=h9891192_7 \
  gxx_linux-64=16.2.0=h223162a_4 \
  gxx_impl_linux-64=16.2.0=h0d273dc_7 \
  libstdcxx-ng=16.2.0=hdf11a46_7 \
  binutils_linux-64=2.46.1=default_h4852527_103 \
  sysroot_linux-64=2.39=hc4b9eeb_6

conda activate "$PREFIX"

command -v gcc
command -v g++
gcc -dumpfullversion
g++ -dumpfullversion
```

Before relying on the package, discover and record the exact compiler paths:

```bash
find "$PREFIX/bin" -maxdepth 1 -type f -o -type l |
  sort |
  grep -E '(gcc|g\+\+|gcov|gcc-(ar|nm|ranlib))'
```

No system-wide compiler alternatives should be changed.

## Option 3: Linuxbrew exact GCC 16.2.0 bottle

Use this only when Linuxbrew is already present and the executable file was
obtained from the verified formula/bottle.

```bash
brew install gcc

GCC_PREFIX="$(brew --prefix gcc)"
"$GCC_PREFIX/bin/g++-16" -dumpfullversion
"$GCC_PREFIX/bin/gcc-16" -dumpfullversion
```

If the formula alias `gcc@16` is supported by the local Linuxbrew:

```bash
brew install gcc@16
GCC_PREFIX="$(brew --prefix gcc@16)"
```

The Linuxbrew bottle SHA256 must be checked through Homebrew's bottle metadata
before execution. The formula stable source checksum is
`e6738e29597f733270731aa90600f37ffdc045079dfc27ec7e8192cc81085c3e`.

## Commands not recommended

- Adding the Ubuntu Toolchain Test PPA.
- `sudo update-alternatives`.
- Copying `/usr/bin/gcc` or `/usr/bin/g++`.
- Using Ubuntu 26.10 `.deb` packages on Ubuntu 22.04 or 24.04.
- Downloading and executing a GCC 16 source archive as the primary solution.
