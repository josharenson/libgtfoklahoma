add_executable(test-game test_game.cpp)
target_include_directories(test-game PUBLIC
        ${CATCH_INCLUDE_DIR}
        ${LIBGTFOKLAHOMA_INCLUDE_DIR})
target_link_libraries(test-game PUBLIC libgtfoklahoma)