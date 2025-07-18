cmake_minimum_required(VERSION 3.13)  # Mise à jour pour correspondre à l'exemple COVESA
project(SOMEIPApps)

# Trouver le package vsomeip (nécessite une installation avec sudo make install)
find_package(vsomeip REQUIRED)

# Trouver Boost avec les composants nécessaires
find_package(Boost REQUIRED COMPONENTS system thread log)

# Définir les fichiers de configuration (similaire à l'exemple COVESA)
set(EXAMPLE_CONFIG_FILES
    "../config/vsomeip-client.json"
    "../config/vsomeip-server.json"
)

# Ajouter l'exécutable server
add_executable(server src/server/server.cpp ${EXAMPLE_CONFIG_FILES})
target_link_libraries(server
    ${vsomeip_LIBRARIES}  # Utilise les bibliothèques trouvées par find_package
    Boost::system
    Boost::thread
    Boost::log
)

# Ajouter l'exécutable client
add_executable(client src/client/client.cpp ${EXAMPLE_CONFIG_FILES})
target_link_libraries(client
    ${vsomeip_LIBRARIES}
    Boost::system
    Boost::thread
    Boost::log
)

# Inclure les répertoires des en-têtes pour vsomeip et Boost
target_include_directories(server PRIVATE ${vsomeip_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_include_directories(client PRIVATE ${vsomeip_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

# Optionnel : Ajouter une dépendance globale (comme dans l'exemple COVESA)
add_custom_target(examples DEPENDS server client)
