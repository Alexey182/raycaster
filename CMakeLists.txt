cmake_minimum_required(VERSION 3.8)

project(raycaster)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -g)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
add_subdirectory(SFML)
find_package(Boost REQUIRED COMPONENTS json)
include_directories(include glad/include)
set(raycaster_SRC
    glad/src/glad.c
    main.cpp
    auxiliary.cpp
    scene.cpp
    renderer.cpp
   )
add_executable(raycaster ${raycaster_SRC})
target_link_libraries(raycaster sfml-graphics GL ${CMAKE_DL_LIBS} ${Boost_LIBRARIES})
