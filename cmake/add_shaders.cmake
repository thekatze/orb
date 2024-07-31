function(add_shaders TARGET_NAME)
    set(SHADER_SOURCE_FILES ${ARGN})

    list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
    if (FILE_COUNT EQUAL 0)
        MESSAGE(FATAL_ERROR "add_shaders called without input")
    endif()

    set(SHADER_COMMANDS)
    set(COMPILED_SHADERS)

    foreach(SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
        cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
        cmake_path(GET SHADER_SOURCE FILENAME SHADER_NAME)

        set(SHADER_OUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.spv")
        list(APPEND SHADER_COMMANDS COMMAND Vulkan::glslc ARGS "${SHADER_SOURCE}" -o "${SHADER_OUT_PATH}")
        list(APPEND COMPILED_SHADERS "${SHADER_OUT_PATH}")
    endforeach()

    add_custom_command(TARGET ${TARGET_NAME} 
        PRE_BUILD
        ${SHADER_COMMANDS}
        COMMENT "Compiling Shaders"
        SOURCES ${SHADER_SOURCE_FILES}
        BYPRODUCTS ${COMPILED_SHADERS}
    )
endfunction()
