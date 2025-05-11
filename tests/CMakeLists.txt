

add_executable(tests main.cpp
        bitboard_tests.cpp
        utils_tests.cpp
)

target_link_libraries(tests PRIVATE
        Kreveta_2_logic
        Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(tests)