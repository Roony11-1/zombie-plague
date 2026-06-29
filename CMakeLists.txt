cmake_minimum_required(VERSION 3.14)
project(zombie_plague LANGUAGES CXX)

# --- Rutas de las dependencias ---
set(REHLDS_PATH    ${CMAKE_SOURCE_DIR}/external/re_hlds/hlsdk)         # SDK básico (tipos del motor)
set(GAMEDLL_PATH   ${CMAKE_SOURCE_DIR}/external/re_game_dll/cssdk)     # SDK completo del juego (entidades)
set(METAMOD_PATH   ${CMAKE_SOURCE_DIR}/external/metamod/sdk)

# --- Interfaz HLSDK mínima (para plugins básicos) ---
add_library(hlsdk_base INTERFACE)
target_include_directories(hlsdk_base INTERFACE
    ${REHLDS_PATH}
    ${REHLDS_PATH}/common
    ${REHLDS_PATH}/dlls
    ${REHLDS_PATH}/engine
    ${REHLDS_PATH}/game_shared
    ${REHLDS_PATH}/pm_shared
    ${REHLDS_PATH}/public
)

# --- Interfaz para el SDK del juego (entidades, utilidades) ---
add_library(hlsdk_game INTERFACE)
target_include_directories(hlsdk_game INTERFACE
    ${GAMEDLL_PATH}
    ${GAMEDLL_PATH}/common
    ${GAMEDLL_PATH}/dlls
    ${GAMEDLL_PATH}/engine
    ${GAMEDLL_PATH}/game_shared
    ${GAMEDLL_PATH}/pm_shared
    ${GAMEDLL_PATH}/public
)

# --- Interfaz Metamod ---
add_library(metamod INTERFACE)
target_include_directories(metamod INTERFACE ${METAMOD_PATH})

# --- Flags de compilación comunes ---
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oi /Oy /GS- /D_CRT_SECURE_NO_WARNINGS")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -msse3 -flto -funroll-loops -fomit-frame-pointer -fno-stack-protector -static-libgcc -static-libstdc++")
endif()

add_subdirectory(infrastructure)
add_subdirectory(plugins)