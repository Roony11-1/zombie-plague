function(target_use_infrastructure TARGET)

    target_sources(${TARGET}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/infrastructure/reapi/src/reapi.cc
            ${PROJECT_SOURCE_DIR}/infrastructure/meta_plugin/src/MetaPlugin.cc
    )

    target_include_directories(${TARGET}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/infrastructure/reapi/include
            ${PROJECT_SOURCE_DIR}/infrastructure/meta_plugin/include
    )

endfunction()