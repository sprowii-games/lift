cmake_minimum_required(VERSION 3.20)

project(LIFT VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
#  raylib 5.5 (same version as raylib/packages.config)
# ---------------------------------------------------------------------------
include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    URL_HASH SHA256=aea98ecf5bc5c5e0b789a76de0083a21a70457050ea4cc2aec7566935f5e258e
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(raylib)

# ---------------------------------------------------------------------------
#  LIFT
# ---------------------------------------------------------------------------
add_executable(LIFT
    raylib/main.cpp
    raylib/player.cpp
    raylib/camera.cpp
)

target_include_directories(LIFT PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/raylib
)
target_link_libraries(LIFT PRIVATE raylib)

if(MSVC)
    target_compile_options(LIFT PRIVATE /utf-8 /W3 /wd4244 /wd4305)
    target_compile_definitions(LIFT PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Desktop: no console window behind the game.
if(WIN32 AND NOT EMSCRIPTEN)
    set_target_properties(LIFT PROPERTIES WIN32_EXECUTABLE TRUE)
    if(MSVC)
        # Keep a regular main() while using the Windows subsystem.
        target_link_options(LIFT PRIVATE /ENTRY:mainCRTStartup)
    endif()
endif()

# ---------------------------------------------------------------------------
#  Web (Emscripten) — everything is packed into index.data
# ---------------------------------------------------------------------------
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set_target_properties(LIFT PROPERTIES OUTPUT_NAME "index" SUFFIX ".html")

    target_link_options(LIFT PRIVATE
        "-sUSE_GLFW=3"
        "-sASYNCIFY"
        "-sFORCE_FILESYSTEM=1"
        "-sINITIAL_MEMORY=134217728"
        "-sSTACK_SIZE=1048576"
        "-sEXPORTED_RUNTIME_METHODS=ccall"
        "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/textures/elevator-closed.png@/textures/elevator-closed.png"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/textures/elevator-opened.png@/textures/elevator-opened.png"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/textures/floor.png@/textures/floor.png"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/textures/note.png@/textures/note.png"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/textures/vinnete.png@/textures/vinnete.png"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/textures/wall.png@/textures/wall.png"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/textures/player@/textures/player"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/sounds@/sounds"
        "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/handjet.ttf@/handjet.ttf"
    )
endif()

# ---------------------------------------------------------------------------
#  Assets next to the binary (desktop only)
# ---------------------------------------------------------------------------
if(NOT EMSCRIPTEN)
    install(TARGETS LIFT RUNTIME DESTINATION .)
    install(DIRECTORY textures sounds DESTINATION .)
    install(FILES handjet.ttf README.md LICENSE DESTINATION .)
endif()
