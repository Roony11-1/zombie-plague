function(target_use_infrastructure TARGET)

    target_sources(${TARGET}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/reapi/src/reapi.cc
    )

    target_include_directories(${TARGET}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/reapi/include
    )

endfunction()