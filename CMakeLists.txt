cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    SaplingTactics
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)


# --- Third Party Libraries

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.2.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

# raylib-cpp
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG v4.2.6
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

# --- Declare ArborMaster Library (TODO: Move to other repo) ---

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

file (GLOB adapter_sources  "./source/adapter/*.cpp" "./source/adapter/*.h")

add_library(
    ArborMaster_adapter OBJECT
    ${adapter_sources}
)

target_link_libraries(ArborMaster_adapter PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(
    ArborMaster_adapter ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_include_directories(
    ArborMaster_adapter ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/adapter>"
)



# ---- Declare library ----

file (GLOB lib_sources  "./source/lib/*.cpp" "./source/lib/*.h")

add_library(
    SaplingTactics_lib OBJECT
    source/lib.hpp
    source/lib.cpp
    ${lib_sources}
)

target_include_directories(
    SaplingTactics_lib ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_include_directories(
    SaplingTactics_lib ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/lib>"
)

target_compile_features(SaplingTactics_lib PUBLIC cxx_std_20)

target_link_libraries(SaplingTactics_lib PUBLIC raylib raylib_cpp ArborMaster_adapter)

# ---- Declare executable ----

add_executable(SaplingTactics_exe source/main.cpp)
add_executable(SaplingTactics::exe ALIAS SaplingTactics_exe)

set_property(TARGET SaplingTactics_exe PROPERTY OUTPUT_NAME SaplingTactics)

target_compile_features(SaplingTactics_exe PRIVATE cxx_std_20)

target_link_libraries(SaplingTactics_exe PRIVATE SaplingTactics_lib)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT SaplingTactics_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of SaplingTactics"
  )
endif()

include(cmake/dev-mode.cmake)
