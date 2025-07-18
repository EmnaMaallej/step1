# Copyright (C) 2015-2023 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.13)

project(vsomeip_app)

set(VSOMEIP_NAME vsomeip3)
set(VSOMEIP_MAJOR_VERSION 3)
set(VSOMEIP_MINOR_VERSION 5)
set(VSOMEIP_PATCH_VERSION 6)
set(VSOMEIP_VERSION ${VSOMEIP_MAJOR_VERSION}.${VSOMEIP_MINOR_VERSION}.${VSOMEIP_PATCH_VERSION})
add_definitions(-DVSOMEIP_VERSION="${VSOMEIP_VERSION}")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Installation directories
include(GNUInstallDirs)
set(INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Installation directory for header files")
set(INSTALL_CMAKE_DIR lib/cmake/${VSOMEIP_NAME} CACHE PATH "Installation directory for CMake files")

# Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Find dependencies
find_package(Threads REQUIRED)
find_package(Boost 1.66 REQUIRED COMPONENTS system thread filesystem)

# OS-specific settings (assuming Linux)
set(OS "LINUX")
add_definitions(-D${OS})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_definitions(-D_GLIBCXX_USE_NANOSLEEP -D_FORTIFY_SOURCE=2)
add_compile_options(
  -Wall
  -Wextra
  -Wformat
  -Wformat-security
  -Wconversion
  -Werror=format-security
  -Wpedantic
  -fexceptions
  -fstrict-aliasing
  -fstack-protector-strong
  -fasynchronous-unwind-tables
  -fno-omit-frame-pointer
)
add_definitions(-D_GTHREAD_USE_MUTEX_INIT_FUNC -D_GTHREAD_USE_RECURSIVE_MUTEX_INIT_FUNC)

# Base library (vsomeip3)
file(GLOB vsomeip_SRC
  "implementation/endpoints/src/*.cpp"
  "implementation/logger/src/*.cpp"
  "implementation/tracing/src/*.cpp"
  "implementation/message/src/*.cpp"
  "implementation/plugin/src/*.cpp"
  "implementation/protocol/src/*.cpp"
  "implementation/routing/src/*.cpp"
  "implementation/runtime/src/*.cpp"
  "implementation/security/src/*.cpp"
  "implementation/utility/src/*.cpp"
)
list(SORT vsomeip_SRC)

add_library(${VSOMEIP_NAME} SHARED ${vsomeip_SRC})
target_compile_features(${VSOMEIP_NAME} PRIVATE cxx_std_17)
target_include_directories(
  ${VSOMEIP_NAME}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/interface>
         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/implementation/configuration/include>
         $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
)
target_include_directories(${VSOMEIP_NAME} SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
set_target_properties(
  ${VSOMEIP_NAME} PROPERTIES
  SOVERSION ${VSOMEIP_MAJOR_VERSION}
  VERSION ${VSOMEIP_VERSION}
  OUTPUT_NAME ${VSOMEIP_NAME}
)
target_link_libraries(
  ${VSOMEIP_NAME}
  PRIVATE Boost::filesystem
          Boost::thread
          Threads::Threads
          dl
)
target_link_options(${VSOMEIP_NAME} PRIVATE "LINKER:-as-needed")

# Configuration library (vsomeip3-cfg)
file(GLOB vsomeip_cfg_SRC "implementation/configuration/src/*.cpp")
list(SORT vsomeip_cfg_SRC)

add_library(${VSOMEIP_NAME}-cfg SHARED ${vsomeip_cfg_SRC})
set_target_properties(
  ${VSOMEIP_NAME}-cfg PROPERTIES
  VERSION ${VSOMEIP_VERSION}
  SOVERSION ${VSOMEIP_MAJOR_VERSION}
  OUTPUT_NAME ${VSOMEIP_NAME}-cfg
)
target_compile_features(${VSOMEIP_NAME}-cfg PRIVATE cxx_std_17)
target_link_libraries(
  ${VSOMEIP_NAME}-cfg
  PRIVATE ${VSOMEIP_NAME}
          Boost::filesystem
          Boost::thread
          Threads::Threads
          dl
)

# Service Discovery library (vsomeip3-sd)
file(GLOB vsomeip_sd_SRC "implementation/service_discovery/src/*.cpp")
list(SORT vsomeip_sd_SRC)

add_library(${VSOMEIP_NAME}-sd SHARED ${vsomeip_sd_SRC})
set_target_properties(
  ${VSOMEIP_NAME}-sd PROPERTIES
  VERSION ${VSOMEIP_VERSION}
  SOVERSION ${VSOMEIP_MAJOR_VERSION}
  OUTPUT_NAME ${VSOMEIP_NAME}-sd
)
target_compile_features(${VSOMEIP_NAME}-sd PRIVATE cxx_std_17)
target_link_libraries(
  ${VSOMEIP_NAME}-sd
  PUBLIC ${VSOMEIP_NAME}
  PRIVATE Threads::Threads
          dl
)

# E2E library (vsomeip3-e2e)
file(GLOB_RECURSIVE vsomeip_e2e_SRC "implementation/e2e_protection/src/*.cpp")
list(SORT vsomeip_e2e_SRC)

add_library(${VSOMEIP_NAME}-e2e SHARED ${vsomeip_e2e_SRC})
set_target_properties(
  ${VSOMEIP_NAME}-e2e PROPERTIES
  VERSION ${VSOMEIP_VERSION}
  SOVERSION ${VSOMEIP_MAJOR_VERSION}
  OUTPUT_NAME ${VSOMEIP_NAME}-e2e
)
target_compile_features(${VSOMEIP_NAME}-e2e PRIVATE cxx_std_17)
target_link_libraries(
  ${VSOMEIP_NAME}-e2e
  PUBLIC ${VSOMEIP_NAME}
  PRIVATE Threads::Threads
          dl
)

# Client and Server executables
add_executable(vsomeip_client src/client.cpp)
target_compile_features(vsomeip_client PRIVATE cxx_std_17)
target_include_directories(vsomeip_client PRIVATE ${PROJECT_SOURCE_DIR}/interface)
target_link_libraries(
  vsomeip_client
  PRIVATE ${VSOMEIP_NAME}
          ${VSOMEIP_NAME}-sd
          ${VSOMEIP_NAME}-e2e
          ${VSOMEIP_NAME}-cfg
          Boost::system
          Boost::thread
          Threads::Threads
)

add_executable(vsomeip_server src/server.cpp)
target_compile_features(vsomeip_server PRIVATE cxx_std_17)
target_include_directories(vsomeip_server PRIVATE ${PROJECT_SOURCE_DIR}/interface)
target_link_libraries(
  vsomeip_server
  PRIVATE ${VSOMEIP_NAME}
          ${VSOMEIP_NAME}-sd
          ${VSOMEIP_NAME}-e2e
          ${VSOMEIP_NAME}-cfg
          Boost::system
          Boost::thread
          Threads::Threads
)

# Configuration files
set(CONFIG_FILES
  "config/vsomeip_client.json"
  "config/vsomeip_server.json"
)

# Installation
install(
  TARGETS ${VSOMEIP_NAME} ${VSOMEIP_NAME}-cfg ${VSOMEIP_NAME}-sd ${VSOMEIP_NAME}-e2e
  EXPORT ${VSOMEIP_NAME}Targets
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
)

install(
  TARGETS vsomeip_client vsomeip_server
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
)

install(
  FILES ${CONFIG_FILES}
  DESTINATION etc/vsomeip
  COMPONENT config
)

# Install headers
file(GLOB_RECURSIVE vsomeip_INCLUDE RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/interface/vsomeip" "interface/*.h*")
list(SORT vsomeip_INCLUDE)
foreach(file ${vsomeip_INCLUDE})
  get_filename_component(dir ${file} DIRECTORY)
  install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/interface/vsomeip/${file}"
    DESTINATION "${INSTALL_INCLUDE_DIR}/vsomeip/${dir}"
    COMPONENT dev
  )
endforeach()

# Install CMake configuration files
configure_file(${VSOMEIP_NAME}Config.cmake.in "${PROJECT_BINARY_DIR}/${VSOMEIP_NAME}Config.cmake" @ONLY)
configure_file(${VSOMEIP_NAME}ConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/${VSOMEIP_NAME}ConfigVersion.cmake" @ONLY)
install(
  FILES "${PROJECT_BINARY_DIR}/${VSOMEIP_NAME}Config.cmake" "${PROJECT_BINARY_DIR}/${VSOMEIP_NAME}ConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
  COMPONENT dev
)
install(
  EXPORT ${VSOMEIP_NAME}Targets
  DESTINATION "${INSTALL_CMAKE_DIR}"
  COMPONENT dev
)

# Configure internal.hpp
configure_file(
  "${PROJECT_SOURCE_DIR}/implementation/configuration/include/internal.hpp.in"
  "${PROJECT_BINARY_DIR}/implementation/configuration/include/internal.hpp"
)
