# GCC 16.2.0 Final Recommendation

## Final status: INSUFFICIENT EVIDENCE

Conda-forge and Linuxbrew have credible exact 16.2.0 package records, but the
research did not execute a compiler, extract a package, test Ubuntu 22.04/24.04,
or build KALAIME dependencies. A standalone portable GCC toolchain is therefore
not verified.

## Answers

1. **Is there a trustworthy GCC 16.2.0 native Linux prebuilt binary?**
   Exact Conda-forge Linux-64 package artifacts and a Linuxbrew bottle record
   are credible, but GCC itself does not distribute an official Linux x86_64
   binary and no candidate is execution-verified here.

2. **Where can it be downloaded?**
   - conda-forge Anaconda.org packages;
   - Homebrew core GCC formula and Linux bottle;
   - Ubuntu 26.10 native APT package.

3. **Does it work on Ubuntu 22.04 and 24.04?**
   Not verified without execution. Conda-forge and Linuxbrew candidates are
   designed for portable Linux environments, but Ubuntu compatibility must be
   tested on the exact hosts.

4. **Can it be installed without source compilation?**
   Yes through Conda-forge or an available Linuxbrew bottle. Both are
   package/bottle distributions. Linuxbrew must use a compatible bottle;
   otherwise it may build from source.

5. **Can it be extracted under `$HOME/toolchains/`?**
   Conda supports a prefix under `$HOME/toolchains/gcc-16.2.0`. Linuxbrew
   normally uses a Linuxbrew prefix rather than an arbitrary toolchain directory,
   so it is less direct.

6. **Can it coexist with existing Ubuntu GCC?**
   Package/environment metadata indicates it can coexist in a prefix without
   changing alternatives, but execution has not verified the claim.

7. **What is the safest installation method?**
   - Ubuntu 26.10: native APT.
   - Ubuntu 22.04/24.04: exact conda-forge package pinned to one build,
     after compatibility testing.

8. **What exact commands should be used?**
   The installation commands are in `INSTALLATION-GUIDE.md`. No installer was
   executed by this research.

9. **What are the limitations?**
   - Conda runtime compatibility is not yet measured.
   - Linuxbrew Linux bottle compatibility is not yet measured.
   - Ubuntu 22.04/24.04 lack official GCC 16 base package evidence.
   - Ubuntu PPA packages are snapshots or unsupported test builds.
   - Official GCC source archives require compilation.

10. **Final recommendation for KALAIME?**
    Do not claim GCC 16.2.0 support until a package is installed in a disposable
    environment and tested on Ubuntu 22.04 and 24.04 with CMake, Ninja,
    GoogleTest, SQLite and ICU. Conda/Miniforge is the current metadata-strongest
    prefix-isolated candidate, but it is only conditionally usable after
    executable/runtime verification.
