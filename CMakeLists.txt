cmake_minimum_required(VERSION 3.25)

project(KALAIME VERSION 0.1.0 LANGUAGES CXX)

option(BUILD_TESTING "Build KALAIME test executables" OFF)
option(KALAIME_BUILD_BENCHMARKS "Build optional Google Benchmark executables" OFF)
option(KALAIME_ENABLE_MSVC_ANALYSIS "Enable strict MSVC warnings and /analyze" OFF)
option(KALAIME_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(KALAIME_BUILD_FUZZING "Build optional libFuzzer targets" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ICU REQUIRED COMPONENTS uc i18n)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_library(kalaime_unicode STATIC src/unicode/bengali.cpp)
target_include_directories(kalaime_unicode PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(kalaime_unicode PUBLIC ICU::uc ICU::i18n)

add_library(kalaime_layout_api INTERFACE)
target_include_directories(kalaime_layout_api INTERFACE "${PROJECT_SOURCE_DIR}/include")
add_library(kalaime_layout_registry STATIC src/layout-api/layout_registry.cpp)
target_include_directories(kalaime_layout_registry PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(kalaime_layout_registry PUBLIC kalaime_layout_api)

add_library(kalaime_phonetic STATIC
    src/layouts/phonetic/kala_phonetic_layout.cpp
    src/layouts/phonetic/phonetic_input_session.cpp
    src/layouts/phonetic/phonetic_parser.cpp
    src/layouts/phonetic/phonetic_tokenizer.cpp
)
target_include_directories(kalaime_phonetic PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(kalaime_phonetic PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(kalaime_phonetic PUBLIC kalaime_layout_api kalaime_unicode)

add_library(kalaime_candidates STATIC src/candidates/candidate_engine.cpp)
target_include_directories(kalaime_candidates PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(kalaime_candidates PUBLIC
    kalaime_layout_api
    kalaime_phonetic
    kalaime_unicode
    kalaime_dictionary
)

add_library(kalaime_dictionary STATIC src/dictionary/dictionary.cpp)
target_include_directories(kalaime_dictionary PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(kalaime_dictionary PUBLIC unofficial::sqlite3::sqlite3)

add_library(kalaime_persistence INTERFACE)
target_link_libraries(kalaime_persistence INTERFACE unofficial::sqlite3::sqlite3)

add_library(kalaime_settings STATIC src/settings/settings.cpp)
target_include_directories(kalaime_settings PUBLIC "${PROJECT_SOURCE_DIR}/include")

add_library(kalaime_core STATIC src/core/project.cpp src/core/engine.cpp)
target_include_directories(kalaime_core PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(kalaime_core
    PUBLIC
        kalaime_unicode
        kalaime_layout_api
        kalaime_phonetic
        kalaime_candidates
        kalaime_dictionary
        kalaime_persistence
        kalaime_layout_registry
        kalaime_settings
)

function(kalaime_configure_target target_name)
    set_target_properties(${target_name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /utf-8)
        if(KALAIME_ENABLE_MSVC_ANALYSIS)
            target_compile_options(${target_name} PRIVATE /analyze)
        endif()
        if(KALAIME_ENABLE_SANITIZERS)
            target_compile_options(${target_name} PRIVATE /fsanitize=address)
            target_link_options(${target_name} PRIVATE /INCREMENTAL:NO)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
        if(KALAIME_ENABLE_SANITIZERS)
            target_compile_options(${target_name} PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer)
            target_link_options(${target_name} PRIVATE
                -fsanitize=address,undefined)
        endif()
    endif()
endfunction()

kalaime_configure_target(kalaime_unicode)
kalaime_configure_target(kalaime_phonetic)
kalaime_configure_target(kalaime_candidates)
kalaime_configure_target(kalaime_layout_registry)
kalaime_configure_target(kalaime_settings)
kalaime_configure_target(kalaime_dictionary)
kalaime_configure_target(kalaime_core)

if(WIN32)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR NOT MSVC_TOOLSET_VERSION STREQUAL "143")
        message(FATAL_ERROR
            "The Windows production target requires the MSVC v143 toolset; "
            "MinGW, GCC, clang-cl, and other toolsets are unsupported.")
    endif()

    find_package(wil CONFIG REQUIRED)
    add_library(kalaime_windows_tsf SHARED
        src/platform/windows/tsf/module_smoke.cpp
    )
    target_include_directories(kalaime_windows_tsf PUBLIC "${PROJECT_SOURCE_DIR}/include")
    target_compile_definitions(kalaime_windows_tsf PRIVATE KALAIME_WINDOWS_TSF_EXPORTS)
    target_link_libraries(kalaime_windows_tsf PRIVATE WIL::WIL)
    kalaime_configure_target(kalaime_windows_tsf)
endif()

include(CTest)
if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(kalaime_unit_tests
        tests/unit/project_smoke_test.cpp
        tests/unit/phonetic_parser_test.cpp
        tests/unit/phonetic_input_session_test.cpp
        tests/unit/phonetic_candidate_test.cpp
        tests/unit/phonetic_tokenizer_test.cpp
        tests/unit/unicode_test.cpp
        tests/unit/engine_test.cpp
        tests/unit/settings_test.cpp
        tests/unit/linguistic_regression_test.cpp
    )
    target_include_directories(kalaime_unit_tests PRIVATE "${PROJECT_SOURCE_DIR}/src")
    target_link_libraries(kalaime_unit_tests PRIVATE kalaime_core GTest::gtest_main)
    kalaime_configure_target(kalaime_unit_tests)
    add_test(NAME kalaime.unit COMMAND kalaime_unit_tests)

    add_executable(kalaime_integration_tests tests/integration/target_graph_smoke_test.cpp)
    target_link_libraries(kalaime_integration_tests PRIVATE kalaime_core GTest::gtest_main)
    kalaime_configure_target(kalaime_integration_tests)
    add_test(NAME kalaime.integration COMMAND kalaime_integration_tests)

    if(WIN32)
        add_executable(kalaime_windows_tests tests/windows/windows_build_smoke_test.cpp)
        target_link_libraries(kalaime_windows_tests PRIVATE kalaime_windows_tsf GTest::gtest_main)
        kalaime_configure_target(kalaime_windows_tests)
        add_test(NAME kalaime.windows.build_smoke COMMAND kalaime_windows_tests)
        set_tests_properties(kalaime.windows.build_smoke PROPERTIES LABELS "windows;non-interactive")
    endif()
endif()

if(KALAIME_BUILD_BENCHMARKS)
    find_package(benchmark CONFIG REQUIRED)
    add_executable(kalaime_benchmarks benchmarks/infrastructure_benchmark.cpp)
    target_link_libraries(kalaime_benchmarks PRIVATE kalaime_core benchmark::benchmark_main)
    kalaime_configure_target(kalaime_benchmarks)
endif()

if(KALAIME_BUILD_FUZZING)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(FATAL_ERROR "The optional libFuzzer target requires a Clang compiler.")
    endif()
    add_executable(kalaime_phonetic_fuzz tests/fuzz/phonetic_fuzz.cpp)
    target_link_libraries(kalaime_phonetic_fuzz PRIVATE kalaime_core)
    target_compile_options(kalaime_phonetic_fuzz PRIVATE
        -fsanitize=fuzzer,address,undefined
        -fno-omit-frame-pointer)
    target_link_options(kalaime_phonetic_fuzz PRIVATE
        -fsanitize=fuzzer,address,undefined)
endif()
