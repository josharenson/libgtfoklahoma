set(CLIENT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/event_observer.cpp
    main.cpp
)

add_executable(sample-client ${CLIENT_SOURCES})
target_include_directories(sample-client PUBLIC
        ${SPDLOG_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(sample-client PUBLIC libgtfoklahoma)