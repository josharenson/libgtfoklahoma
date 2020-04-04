add_executable(test-game
        run.cpp
        test_actions.cpp
        test_events.cpp
        test_game.cpp
        test_issues.cpp
        test_items.cpp
        test_rules.cpp
        test_stats.cpp)

target_include_directories(test-game PUBLIC
        ${CATCH_INCLUDE_DIR}
        ${FAKEIT_INCLUDE_DIR}
        ${LIBGTFOKLAHOMA_INCLUDE_DIR})
target_link_libraries(test-game PUBLIC libgtfoklahoma)
