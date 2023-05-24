cmake_minimum_required(VERSION 3.8)

project(raycaster)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -g)
add_subdirectory(SFML-2.5.1)
#find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(Boost REQUIRED COMPONENTS json)
include_directories(include glad/include)
set(raycaster_SRC
    glad/src/glad.c
    main.cpp
    scene.cpp
    renderer.cpp
   )
add_executable(raycaster ${raycaster_SRC})
target_link_libraries(raycaster sfml-graphics GL ${CMAKE_DL_LIBS} ${Boost_LIBRARIES})
