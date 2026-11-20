# KALAIME Production Readiness Blockers

## Active blockers

1. `VCPKG_ROOT` is missing; the existing Ubuntu verification script cannot configure.
2. Hosted environment has no Windows SDK/MSVC runtime; real TSf/COM verification is unavailable.
3. Core Engine orchestration and layout registry are not implemented.
4. SQLite candidate/dictionary integration is incomplete.
5. Interactive Windows application compatibility is untested.
6. Performance numerical targets are not yet measured.

## Recovery

- Provision the repository-defined pinned vcpkg checkout.
- Run existing Ubuntu verification.
- Continue portable architecture work only after baseline tests pass.
