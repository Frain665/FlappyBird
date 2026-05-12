cmake_minimum_required(VERSION 3.20)

project(FlappyBird
    VERSION 1.0.0
    DESCRIPTION "Flappy Bird style game with raylib"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- raylib: системная установка (vcpkg, пакет) или автозагрузка ---
find_package(raylib 5.0 QUIET CONFIG)

if(NOT raylib_FOUND)
    include(FetchContent)

    # Проект в пути с кириллицей: MSBuild при FetchContent часто портит путь к
    # build/_deps и падает на «Failed to remove directory». Кладём зависимости
    # в каталог только с латиницей (обычно уже ASCII).
    if(DEFINED ENV{LOCALAPPDATA})
        file(TO_CMAKE_PATH "$ENV{LOCALAPPDATA}/FlappyBird_cmake_deps" FLAPPY_FETCHCONTENT_BASE)
    else()
        set(FLAPPY_FETCHCONTENT_BASE "C:/FlappyBird_cmake_deps")
    endif()
    set(FETCHCONTENT_BASE_DIR "${FLAPPY_FETCHCONTENT_BASE}"
        CACHE PATH "FetchContent: Latin-only path avoids MSVC/git step errors with Cyrillic source dirs"
        FORCE)

    message(STATUS "FetchContent / raylib sources: ${FETCHCONTENT_BASE_DIR}")

    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
    )
    FetchContent_MakeAvailable(raylib)
endif()

set(FLAPPY_SOURCES
    src/main.cpp
    src/Bird.cpp
    src/Pipe.cpp
    src/PipeManager.cpp
    src/DifficultyManager.cpp
    src/Game.cpp
)

add_executable(${PROJECT_NAME} ${FLAPPY_SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(TARGET raylib::raylib)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib::raylib)
elseif(TARGET raylib)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
else()
    message(FATAL_ERROR "raylib target not found after find_package / FetchContent")
endif()

# Строки с кириллицей в Game.cpp (UTF-8)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()
