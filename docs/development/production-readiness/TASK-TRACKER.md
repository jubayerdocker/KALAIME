# KALAIME Task Tracker

| ID | Task | Status | Dependencies | Evidence |
|---|---|---|---|---|
| P0-1 | Complete repository audit | done | none | Source/build/AI ecosystem audit |
| P0-2 | Restore pinned Ubuntu dependencies | done | network/toolchain access | vcpkg baseline 0df93a05 |
| P0-3 | Run baseline Ubuntu build/tests | done | P0-2 | 2/2 CTest passed |
| P0-4 | Run baseline formatting/tidy checks | done | P0-2 | format/tidy executed; warnings remain |
| P1-1 | Central Engine API | done | P0-3 | Engine, layout registry and session factory |
| P1-2 | Layout registry and independent sessions | done | P1-1 | Layout injection and switching tests |
| P3-1 | Dictionary/candidate integration | done | P1-2 | Exact SQLite matches merged and deduplicated |
| P6-1 | Settings and persistence | done | P0-3 | Save/load/default/malformed/unsupported tests pass |
| P3-2 | Dictionary migrations/recovery | done | P3-1 | Corrupt, migration and prefix tests pass |
| P7-1 | Sanitizer and fuzz gates | done | P0-3 | ASan/UBSan CTest 2/2; 1,000 fuzz runs |
| P8-1 | Performance baseline | done | P7-1 | Bounded Clang 23.1.5 Release evidence |
| P9-1 | Dependency security scan | done | P0-3 | No vulnerabilities found |
| P5-1 | Windows COM/TSF implementation | pending | P1-1/P3-1 | no implementation |
