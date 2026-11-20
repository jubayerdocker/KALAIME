# GCC 16.2.0 Security and Verification

## Official GCC source archive

The official GCC 16.2.0 directory provides `.sig` detached signatures. GNU
archive integrity also requires verification against a trusted release signing
key and a correctly downloaded signature.

Official source archive URLs:

- `https://ftp.gnu.org/gnu/gcc/gcc-16.2.0/gcc-16.2.0.tar.gz`
- `https://ftp.gnu.org/gnu/gcc/gcc-16.2.0/gcc-16.2.0.tar.gz.sig`
- `https://ftp.gnu.org/gnu/gcc/gcc-16.2.0/gcc-16.2.0.tar.xz`
- `https://ftp.gnu.org/gnu/gcc/gcc-16.2.0/gcc-16.2.0.tar.xz.sig`

No source or binary checksum should be invented. Verify the archive with both
its official signature and an independently obtained checksum policy.

## Conda-forge verification

Conda/Anaconda channel artifacts provide:

- package SHA256;
- package MD5;
- build number;
- target subdir;
- target triplet;
- package dependencies;
- package license metadata.

For linux-64 build `h9891192_7`, for example:

- artifact:
  `gcc_impl_linux-64-16.2.0-h9891192_7.conda`
- SHA256:
  `14acaa661e4c4efc65606612b8b08f8f079c6a9e6da3ffcb77056685c675651`

Conda's integrity check should be performed by Conda itself. The channel's
Anaconda API record should be used as a package-repository inventory, not as a
replacement for package signing and environment verification.

## Linuxbrew/Homebrew verification

The Linux x86_64 bottle record provides:

- SHA256:
  `6504ecae947061ffe79c9dea2f635791bf54b148d1063ec857873eac2a67d922`
- source checksum:
  `e6738e29597f733270731aa90600f37ffdc045079dfc27ec7e8192cc81085c3e`

Homebrew itself should verify bottle metadata and checksums. The source formula
should be reviewed from the homebrew-core repository and pinned/reviewed before
installation.

## Supply-chain requirements

- Record exact package/build names and checksums.
- Pin channels and build numbers.
- Verify package licenses:
  - GCC implementation: GPL-3.0-only with GCC Runtime Library Exception;
  - Conda activation packages: BSD-3-Clause;
  - binutils: GPL;
  - sysroot package: mixed LGPL/GPL license metadata.
- Generate an SBOM for the selected environment.
- Do not execute installer scripts or unknown binaries.
- Do not add PPA signing keys or repositories without explicit approval.
- Do not modify `/usr/bin/gcc`, `/usr/bin/g++` or alternatives.
