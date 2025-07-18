# Copyright (C) 2015-2017 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.13)
project(SOMEIPApps)

find_package(Threads REQUIRED)
find_package(vsomeip REQUIRED)  # Remplace ${VSOMEIP_NAME} par une détection explicite
find_package(Boost REQUIRED COMPONENTS system thread log)

include(GNUInstallDirs)

# Définir les fichiers de configuration
set(EXAMPLE_CONFIG_FILES
    "../config/vsomeip-client.json"
    "../config/vsomeip-server.json"
)

# Fonction pour créer une cible d'interface
function(create_target target_name)
    add_library(vsomeip_${target_name}_interface INTERFACE)
    # Pas d'en-tête spécifique ici, car tes fichiers .cpp sont autonomes
    target_compile_features(vsomeip_${target_name}_interface INTERFACE cxx_std_17)
    target_include_directories(vsomeip_${target_name}_interface INTERFACE
        ${PROJECT_SOURCE_DIR}
        ${vsomeip_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
    )
endfunction()

# Fonction pour lier une cible exécutable
function(link_target target_name)
    add_executable(${target_name} ${EXAMPLE_CONFIG_FILES})
    target_sources(${target_name} PRIVATE src/${target_name}/${target_name}.cpp)
    target_link_libraries(${target_name} PRIVATE
        vsomeip_${target_name}_interface
        ${vsomeip_LIBRARIES}
        Threads::Threads
        Boost::system
        Boost::thread
        Boost::log
    )
    if(ENABLE_SIGNAL_HANDLING)
        target_compile_definitions(vsomeip_${target_name}_interface PRIVATE VSOMEIP_ENABLE_SIGNAL_HANDLING)
    endif()
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endfunction()

# Créer et lier les cibles
create_target("client")
create_target("server")

link_target("client")
link_target("server")

# Optionnel : Ajouter une cible globale
add_custom_target(examples DEPENDS client server)

# Installation (facultatif, selon tes besoins)
install(
    TARGETS client server
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
    COMPONENT example-someipapps
)
