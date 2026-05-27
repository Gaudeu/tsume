

cmake_minimum_required (VERSION 3.10)


if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("PC_server")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WINSOCK_LIB ws2_32)

# (.h)
set(VIGEM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/include")

#  .lib 
set(VIGEM_LIB "${CMAKE_CURRENT_SOURCE_DIR}/deps/include/lib/x64/ViGEmClient.lib")


add_executable(ControllerServer PC_server/main.cpp) 

# Configura as pastas de inclusão e as bibliotecas para o Linker
target_include_directories(ControllerServer PRIVATE ${VIGEM_INCLUDE_DIR})
target_link_libraries(ControllerServer PRIVATE ${WINSOCK_LIB} ${VIGEM_LIB})

set_target_properties(ControllerServer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")