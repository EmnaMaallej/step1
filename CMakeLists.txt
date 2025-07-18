cmake_minimum_required(VERSION 3.10)
project(SOMEIPApps)

find_library(VSOMEIP_LIBRARY vsomeip3)
find_library(VSOMEIP_SD_LIBRARY vsomeip3-sd)
find_package(Boost REQUIRED COMPONENTS system thread log)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(server src/server/server.cpp)
target_link_libraries(server ${VSOMEIP_LIBRARY} ${VSOMEIP_SD_LIBRARY} ${Boost_LIBRARIES})

add_executable(client src/client/client.cpp)
target_link_libraries(client ${VSOMEIP_LIBRARY} ${VSOMEIP_SD_LIBRARY} ${Boost_LIBRARIES})
