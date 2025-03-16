cmake_minimum_required(VERSION 3.16)
project(Ez_voder VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(SOURCES
    main.cpp
    mainwindow.cpp
)

set(HEADERS
    mainwindow.h
)

set(RESOURCES
    style.qrc
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
)

configure_file(${CMAKE_SOURCE_DIR}/twitch-dl.pyz ${CMAKE_BINARY_DIR}/twitch-dl.pyz COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/setup_youtube.py ${CMAKE_BINARY_DIR}/setup_youtube.py COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/upload_youtube.py ${CMAKE_BINARY_DIR}/upload_youtube.py COPYONLY)
if(EXISTS "${CMAKE_SOURCE_DIR}/client_secrets.json")
    configure_file(${CMAKE_SOURCE_DIR}/client_secrets.json ${CMAKE_BINARY_DIR}/client_secrets.json COPYONLY)
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/scripts) 