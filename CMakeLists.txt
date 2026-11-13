cmake_minimum_required(VERSION 3.22)

project(DPA_LBMFluids
    VERSION 0.1.0
    LANGUAGES CXX
)

# Houdini 22 uses C++20 for its SDK.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin"
)

# Allow the solver to be built without the standalone viewer and its
# OpenImageIO / OpenGL dependencies.
option(DPALBM_BUILD_VIEWER "Build the standalone LBM viewer" ON)

find_package(OpenMP REQUIRED)

find_package(Boost REQUIRED)
find_package(TBB CONFIG REQUIRED)




# ------------------------------------------------------------
# Utility: header-only code
# ------------------------------------------------------------

add_library(dpalbm_utility INTERFACE)

target_include_directories(dpalbm_utility
    INTERFACE
        # Allows #include "utility/nd_vector.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"

        # Also allows the current style: #include "nd_vector.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/utility"
)

#### if no longer header only, use: ####
# file(GLOB_RECURSE DPALBM_UTILITY_SOURCES CONFIGURE_DEPENDS
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/*.cpp"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/utility/*.C"
# )

# add_library(dpalbm_utility STATIC
#     ${DPALBM_UTILITY_SOURCES}
# )

# target_include_directories(dpalbm_utility
#     PUBLIC
#         "${CMAKE_CURRENT_SOURCE_DIR}/include"
#         "${CMAKE_CURRENT_SOURCE_DIR}/include/utility"
# )

# set_target_properties(dpalbm_utility PROPERTIES
#     POSITION_INDEPENDENT_CODE ON
# )


# ------------------------------------------------------------
# Core LBM solver
# ------------------------------------------------------------

file(GLOB_RECURSE DPALBM_CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lbm_core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lbm_core/*.C"
)

add_library(dpalbm_core STATIC
    ${DPALBM_CORE_SOURCES}
)

target_include_directories(dpalbm_core
    PUBLIC
        # Allows #include "lbm_core/lbm_data.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"

        # Also allows #include "lbm_data.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/lbm_core"
)

target_link_libraries(dpalbm_core
    PUBLIC
        dpalbm_utility
        OpenMP::OpenMP_CXX
        Boost::headers
        TBB::tbb
)

# Required when this static library is linked into a shared library,
# such as a Houdini plugin.
set_target_properties(dpalbm_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# ------------------------------------------------------------
# HDK
# ------------------------------------------------------------
if(!DPALBM_BUILD_VIEWER)
    list(APPEND CMAKE_PREFIX_PATH "$ENV{HFS}/toolkit/cmake")
    find_package(Houdini REQUIRED)
    set(DPALBM_BUILD_VIEWER OFF CACHE BOOL "" FORCE)
    set(IMAGE_PROCESSING_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(IMAGE_PROCESSING_BUILD_PYTHON OFF CACHE BOOL "" FORCE)


endif()

# ------------------------------------------------------------
# Standalone viewer and executable
# ------------------------------------------------------------

if(DPALBM_BUILD_VIEWER)

    # Keep your existing OpenImageIO/OpenEXR configuration here if needed.
    set(OpenImageIO_ROOT "$ENV{HOME}/local/openimageio" CACHE PATH "")
    set(OpenEXR_ROOT "$ENV{HOME}/local/openexr" CACHE PATH "")

    # These control the third-party project's extra targets.
    set(IMAGE_PROCESSING_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(IMAGE_PROCESSING_BUILD_PYTHON OFF CACHE BOOL "" FORCE)

    add_subdirectory(
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/DPA_ImageProcessing"
        "${CMAKE_CURRENT_BINARY_DIR}/DPA_ImageProcessing"
    )

    file(GLOB_RECURSE DPALBM_VIEWER_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/viewer/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/viewer/*.C"
    )

    add_library(dpalbm_viewer STATIC
        ${DPALBM_VIEWER_SOURCES}
    )

    target_include_directories(dpalbm_viewer
        PUBLIC
            # Allows #include "viewer/lbm_viewer2D.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include"

            # Also allows #include "lbm_viewer2D.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/viewer"
    )

    target_link_libraries(dpalbm_viewer
        PUBLIC
            dpalbm_core
            image_processing
    )

    add_executable(dpa_lbm_fluids
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    )

    target_link_libraries(dpa_lbm_fluids
        PRIVATE
            dpalbm_viewer
    )

endif()





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

    target_compile_definitions(dpalbm_tests PRIVATE
        SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(dpalbm_tests
        PRIVATE
            dpalbm_core
            Catch2::Catch2WithMain
    )

    # Catch2's CMake test-discovery helper is in its extras directory.
    list(APPEND CMAKE_MODULE_PATH
        ${catch2_SOURCE_DIR}/extras
    )

    include(Catch)

    catch_discover_tests(dpalbm_tests)

endif()