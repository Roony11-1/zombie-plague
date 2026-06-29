function(target_use_infrastructure TARGET)

    target_sources(${TARGET}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/infrastructure/reapi/src/reapi.cc
    )

    target_include_directories(${TARGET}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/infrastructure/reapi/include
    )

endfunction()