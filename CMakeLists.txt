cmake_minimum_required(VERSION 3.22)

project(DPA_LBMFluids
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin"
)

# ------------------------------------------------------------
# QT stuff!
# ------------------------------------------------------------

# Qt's CMake integration handles moc/uic/rcc for Qt classes and forms.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(OpenMP REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

# Optional: build tests by default
include(CTest)

# ------------------------------------------------------------
# Core LBM Things!
# ------------------------------------------------------------

file(GLOB_RECURSE DPALBM_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.C"
)

file(GLOB_RECURSE DPALBM_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(dpalbm STATIC
    ${DPALBM_SOURCES}
    ${DPALBM_HEADERS}
)

target_include_directories(dpalbm
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(dpalbm
    PUBLIC
        OpenMP::OpenMP_CXX
)

# ------------------------------------------------------------
# Example/demo executable
# ------------------------------------------------------------

file(GLOB_RECURSE GUI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/gui/src/*.cpp"
)

file(GLOB_RECURSE GUI_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/gui/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/gui/include/*.hpp"
)

add_executable(dpa_lbm_fluids
    ${GUI_SOURCES}
    ${GUI_HEADERS}
)

target_include_directories(dpa_lbm_fluids
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/include"
)

target_link_libraries(dpa_lbm_fluids
    PRIVATE
        dpalbm
        Qt6::Widgets
        Qt6::Charts
)

# ------------------------------------------------------------
# Catch2 tests
# ------------------------------------------------------------

if(BUILD_TESTING)

    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.1
    )

    # Prevent Catch2's own tests from being built.
    set(CATCH_BUILD_TESTING OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(Catch2)

    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    add_executable(dpalbm_tests
        ${TEST_SOURCES}
    )

    target_link_libraries(dpalbm_tests
        PRIVATE
            dpalbm
            Catch2::Catch2WithMain
    )

    # Catch2's CMake test-discovery helper is in its extras directory.
    list(APPEND CMAKE_MODULE_PATH
        ${catch2_SOURCE_DIR}/extras
    )

    include(Catch)

    catch_discover_tests(dpalbm_tests)

endif()