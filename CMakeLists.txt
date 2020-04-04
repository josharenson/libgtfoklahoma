cmake_minimum_required(VERSION 3.14)
project("libGTFOklahoma")
set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)

# 3P Includes
set(3P_DIR ${CMAKE_SOURCE_DIR}/third_party)
set(CATCH_INCLUDE_DIR ${3P_DIR}/Catch2/single_include)
set(FAKEIT_INCLUDE_DIR ${3P_DIR}/FakeIt/single_header)
set(RAPID_JSON_INCLUDE_DIR ${3P_DIR}/rapidjson/include)
set(SPDLOG_INCLUDE_DIR ${3P_DIR}/spdlog/include)
set(SQLITE_ORM_INCLUDE_DIR ${3P_DIR}/sqlite_orm/include)

# Local includes
set(GAMEDATA_DIR ${CMAKE_SOURCE_DIR}/data)
set(LIBGTFOKLAHOMA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# libgtfoklahoma
set(LIBGTFOKLAHOMA_SOURCES
    ${CMAKE_SOURCE_DIR}/src/action_model.cpp
    ${CMAKE_SOURCE_DIR}/src/actions.cpp
    ${CMAKE_SOURCE_DIR}/src/engine.cpp
    ${CMAKE_SOURCE_DIR}/src/event_model.cpp
    ${CMAKE_SOURCE_DIR}/src/events.cpp
    ${CMAKE_SOURCE_DIR}/src/game.cpp
    ${CMAKE_SOURCE_DIR}/src/issue_model.cpp
    ${CMAKE_SOURCE_DIR}/src/issues.cpp
    ${CMAKE_SOURCE_DIR}/src/item_model.cpp
    ${CMAKE_SOURCE_DIR}/src/items.cpp
    ${CMAKE_SOURCE_DIR}/src/rules.cpp
    ${CMAKE_SOURCE_DIR}/src/stats.cpp)

add_library(libgtfoklahoma STATIC ${LIBGTFOKLAHOMA_SOURCES})
target_include_directories(libgtfoklahoma PUBLIC
        ${GAMEDATA_DIR}
        ${LIBGTFOKLAHOMA_INCLUDE_DIR}
        ${RAPID_JSON_INCLUDE_DIR}
        ${SPDLOG_INCLUDE_DIR}
        ${SQLITE_ORM_INCLUDE_DIR})

add_subdirectory(sample_client)
add_subdirectory(test)
