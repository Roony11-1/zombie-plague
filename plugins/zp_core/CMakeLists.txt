set(PLUGIN_NAME zp_core)
set(SOURCES main.cpp)

add_library(${PLUGIN_NAME} SHARED ${SOURCES})

# Enlazar el SDK base y Metamod
target_link_libraries(${PLUGIN_NAME} PRIVATE hlsdk_base metamod)

# Resto de configuración...
target_include_directories(${PLUGIN_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PLUGIN_NAME} PROPERTIES
    LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/exports.def"
    PREFIX ""
    SUFFIX "_mm.dll"
)

target_compile_features(${PLUGIN_NAME} PRIVATE cxx_std_17)
set_target_properties(${PLUGIN_NAME} PROPERTIES CXX_EXTENSIONS OFF)

if(WIN32)
    target_compile_definitions(${PLUGIN_NAME} PRIVATE _WINDOWS WIN32)
else()
    target_compile_definitions(${PLUGIN_NAME} PRIVATE linux __linux__)
endif()