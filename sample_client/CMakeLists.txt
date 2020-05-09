if (BUILD_SAMPLE_CLIENT)
    include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
    # Build Termbox
    ExternalProject_Add(Termbox
        GIT_REPOSITORY "https://github.com/nsf/termbox"
        CONFIGURE_COMMAND
            cd <SOURCE_DIR> && ./waf configure
        BUILD_COMMAND
            cd <SOURCE_DIR> && ./waf build
        INSTALL_COMMAND
            cd <SOURCE_DIR> &&
            ./waf install
                --targets=termbox_static
                --destdir=<INSTALL_DIR> &&
            mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/include &&
            cp <SOURCE_DIR>/src/termbox.h ${CMAKE_CURRENT_SOURCE_DIR}/include/
    )

    set(CLIENT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/event_observer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/window.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/window_animation.cpp
        main.cpp
    )
    ExternalProject_Get_Property(Termbox INSTALL_DIR)
    set(TERMBOX_LINK_DIR ${INSTALL_DIR}/usr/local/lib)

    add_executable(sample-client ${CLIENT_SOURCES})
    target_include_directories(sample-client PUBLIC
            ${SPDLOG_INCLUDE_DIR}
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include)

    target_link_directories(sample-client PUBLIC ${TERMBOX_LINK_DIR})
    target_link_libraries(sample-client libgtfoklahoma termbox)
endif(BUILD_SAMPLE_CLIENT)
