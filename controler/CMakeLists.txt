cmake_minimum_required(VERSION 3.10)

project(Control)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#ferramentas
set(DEVKITPRO "$ENV{DEVKITPRO}")
if(NOT DEFINED ENV{DEVKITPRO})
    message(FATAL_ERROR "A variável DEVKITPRO não foi encontrada no ambiente. Verifique seu .bashrc.")
endif()

set(CTR_3DSXTOOL_EXE "/opt/devkitpro/tools/bin/3dsxtool")
set(CTR_SMDHTOOL_EXE "/opt/devkitpro/tools/bin/smdhtool")
set(CTR_TEX3DS_EXE   "/opt/devkitpro/tools/bin/tex3ds")

#diretorios
include_directories("${DEVKITPRO}/libctru/include" "${DEVKITPRO}/portlibs/3ds/include")
link_directories("${DEVKITPRO}/portlibs/3ds/lib")
include_directories("source")

#
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/romfs/gfx")

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/romfs/gfx/sprites.t3x"
    COMMAND "${CTR_TEX3DS_EXE}" "${CMAKE_CURRENT_SOURCE_DIR}/gfx/sprites.png" -o "${CMAKE_CURRENT_SOURCE_DIR}/romfs/gfx/sprites.t3x"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/gfx/sprites.png"
    VERBATIM
    COMMENT "Convertendo sprites.png para formato 3DS..."
)






add_executable(Control 
    source/main.cpp 
    source/NetworkManager.cpp 
    source/InputHandler.cpp 
    source/cenaPrincipal.cpp 
    source/cenaTexto.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/romfs/gfx/sprites.t3x"
 "source/botao.h"   "source/Widget.h" "source/painel.h" "source/painelTopo.h" "source/WidgetBateria.h" "source/painelRodape.h" "source/cenaConectado.h" "source/cenaConectado.cpp" "source/saveConfig.cpp" "source/saveConfig.h")


target_link_libraries(Control citro2d citro3d ctru m png z)

function(add_3dsx_target target)
    set(SMDH_FILE "${CMAKE_CURRENT_BINARY_DIR}/${target}.smdh")
    set(OUTPUT_3DSX "${CMAKE_CURRENT_BINARY_DIR}/${target}.3dsx")
    set(ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/icon.png") 

    add_custom_command(TARGET ${target} POST_BUILD
    
        # 1.Curto 2.Longo 3.Autor 4.Ícone 5.ArquivoSaída
        COMMAND "${CTR_SMDHTOOL_EXE}" --create "Controle3DS" "Remote Controller for PC" "Gaud" "${ICON_PATH}" "${SMDH_FILE}"
        
        # Gerar o executavel usando o SMDH e o ROMFS
        COMMAND "${CTR_3DSXTOOL_EXE}" "$<TARGET_FILE:${target}>" "${OUTPUT_3DSX}" "--smdh=${SMDH_FILE}" "--romfs=${CMAKE_CURRENT_SOURCE_DIR}/romfs"
        
        COMMENT "Gerando arquivo executavel..."
    )
endfunction()


add_3dsx_target(Control)