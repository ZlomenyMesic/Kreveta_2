cmake_minimum_required(VERSION 3.31)
project(Kreveta_2)

set(CMAKE_CXX_STANDARD 26)

# this allows us to only compile some code in debug build
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.3
)
FetchContent_MakeAvailable(Catch2)

# we must also build the project as a library to allow unit tests
add_library(Kreveta_2_logic
        src/uci.h
        src/uci.cpp
        src/utils.h
        src/bitboard.h
        src/global/consts.cpp
        src/global/consts.h
        src/global/types.h
        src/movegen/move.cpp
        src/movegen/move.h
        src/board.cpp
        src/board.h
        src/position.cpp
        src/position.h
)
target_include_directories(Kreveta_2_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# this is the main executable
add_executable(Kreveta_2 src/main.cpp
        src/uci.h
        src/uci.cpp
        src/utils.h
        src/bitboard.h
        src/global/consts.cpp
        src/global/consts.h
        src/global/types.h
        src/movegen/move.cpp
        src/movegen/move.h
        src/board.cpp
        src/board.h
        src/position.cpp
        src/position.h
)
target_link_libraries(Kreveta_2 PRIVATE Kreveta_2_logic)

enable_testing()
add_subdirectory(tests)