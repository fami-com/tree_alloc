cmake_minimum_required(VERSION 3.19)
project(alloc)

set(CMAKE_CXX_STANDARD 20)

set(allocator_SOURCES inc/arena.h inc/header.h inc/allocator.h inc/rbtree/rbtree.h inc/mem/pimem.h inc/test.h src/allocator.cpp src/header.cpp src/arena.cpp src/main.cpp src/rbtree/rbtree.cpp src/test.cpp)

include_directories(inc)

if(WIN32)
    set(allocator_SOURCES ${allocator_SOURCES} src/mem/winmem.cpp)
elseif(UNIX)
    set(allocator_SOURCES ${allocator_SOURCES} src/mem/unixmem.cpp)
endif()

add_executable(alloc ${allocator_SOURCES})
