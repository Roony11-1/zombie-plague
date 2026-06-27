# Recopilar todos los .cpp del directorio actual
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

# Excluir ex_rehlds_api.cpp (no tenemos la biblioteca rehlds_api.lib)
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ex_rehlds_api.cpp")

# Crear la DLL
add_library(example_plugin SHARED ${SOURCES})

# Enlazar con la interfaz básica (ReHLDS) y Metamod
target_link_libraries(example_plugin PRIVATE hlsdk_base metamod)

# Directorio actual para includes locales
target_include_directories(example_plugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Nombre de salida: example_plugin_mm.dll / .so
if(WIN32)
    set_target_properties(example_plugin PROPERTIES PREFIX "" SUFFIX "_mm.dll")
else()
    set_target_properties(example_plugin PROPERTIES PREFIX "" SUFFIX "_mm_i386.so")
endif()

# C++17 (necesario para headers modernos del HLSDK)
target_compile_features(example_plugin PRIVATE cxx_std_17)
set_target_properties(example_plugin PROPERTIES CXX_EXTENSIONS OFF)

set_target_properties(example_plugin PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/exports.def")

# Definiciones de plataforma
if(WIN32)
    target_compile_definitions(example_plugin PRIVATE _WINDOWS WIN32)
else()
    target_compile_definitions(example_plugin PRIVATE linux __linux__)
endif()