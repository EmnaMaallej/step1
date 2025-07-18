cmake_minimum_required(VERSION 3.10)
project(SOMEIPApps)

# Trouver le package vsomeip (nécessite une installation avec sudo make install)
find_package(vsomeip REQUIRED)

# Trouver Boost avec les composants nécessaires
find_package(Boost REQUIRED COMPONENTS system thread log)

# Inclure les répertoires des en-têtes pour vsomeip et Boost
include_directories(${vsomeip_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

# Ajouter l'exécutable server
add_executable(server src/server/server.cpp)
target_link_libraries(server ${vsomeip_LIBRARIES} ${Boost_LIBRARIES})

# Ajouter l'exécutable client
add_executable(client src/client/client.cpp)
target_link_libraries(client ${vsomeip_LIBRARIES} ${Boost_LIBRARIES})
