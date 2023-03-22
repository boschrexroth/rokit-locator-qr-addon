cmake_minimum_required(VERSION 3.0.0)
project(DM_add_on VERSION 0.1.0)

include(CTest)
enable_testing()

if (NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
        "Build type" FORCE)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g2 -ggdb")
endif()

message(STATUS "Build type:${CMAKE_BUILD_TYPE}")

find_package(RapidJSON REQUIRED)

include_directories(SYSTEM include ${RapidJSON_INCLUDE_DIRS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

aux_source_directory(src SRC_LIST)
add_executable(DM_add_on ${SRC_LIST})
target_link_libraries(DM_add_on ${CMAKE_THREAD_LIBS_INIT} yaml-cpp sqlite3 ${RapidJSON_LIBRARIES} curl pthread uuid spdlog)

