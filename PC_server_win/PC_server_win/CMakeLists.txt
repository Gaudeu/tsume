# CMakeList.txt: projeto de CMake para PC_server, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.
add_executable (PC_server "PC_server.cpp" "PC_server.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PC_server PROPERTY CXX_STANDARD 20)
endif()

# TODO: Adicione testes e instale destinos, se necessário.
