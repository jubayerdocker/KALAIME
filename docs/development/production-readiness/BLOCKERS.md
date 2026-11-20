# SUBORNO Production Readiness Blockers

## Active blockers

1. Hosted environment has no Windows SDK/MSVC runtime; real TSF/COM verification is unavailable.
2. GCC16 portability CI is not installed or executed.
3. Interactive Windows application compatibility is untested.
4. Windows COM/TSF implementation is not implemented.
5. Installer, signing and clean-machine verification are not implemented.
6. IBus and Fcitx 5 development libraries are not installed.
7. macOS SDK and InputMethodKit host are unavailable.

## Recovery

- Provision the repository-defined pinned vcpkg checkout.
- Run existing Ubuntu verification.
- Continue portable architecture work only after baseline tests pass.
