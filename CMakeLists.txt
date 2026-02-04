cmake_minimum_required(VERSION 3.31)
project(lje-imgui CXX)

# Parse version from CHANGELOG.md
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.md" CHANGELOG_CONTENTS)
string(REGEX MATCH "## \\[([0-9]+)\\.([0-9]+)\\.([0-9]+)\\]" VERSION_MATCH "${CHANGELOG_CONTENTS}")
set(PROJECT_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(PROJECT_VERSION_MINOR "${CMAKE_MATCH_2}")
set(PROJECT_VERSION_PATCH "${CMAKE_MATCH_3}")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/src/version.h"
)

set(CMAKE_CXX_STANDARD 20)

# imgui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_dx9.cpp
        ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
)
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC d3d9)

# imnodes
set(IMNODES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/imnodes)
add_library(imnodes STATIC ${IMNODES_DIR}/imnodes.cpp)
target_include_directories(imnodes PUBLIC ${IMNODES_DIR})
target_link_libraries(imnodes PUBLIC imgui)

# MinHook
add_subdirectory(libs/MinHook)

# Main library
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_library(lje-imgui SHARED ${SOURCES} ${HEADERS})
target_include_directories(lje-imgui PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/lje/sdk/include"
        "${CMAKE_CURRENT_BINARY_DIR}/src"
)
target_link_libraries(lje-imgui PRIVATE imgui imnodes minhook d3d9 dxguid)
