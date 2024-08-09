function(add_shaders TARGET_NAME)
    set(SHADER_SOURCE_FILES ${ARGN})

    list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
    if (FILE_COUNT EQUAL 0)
        MESSAGE(FATAL_ERROR "add_shaders called without input")
    endif()

    set(SHADER_COMMANDS)
    set(COMPILED_SHADERS)

    set(SHADER_OUT_DIRECTORY "${CMAKE_BINARY_DIR}/compiled-shaders/${TARGET_NAME}-shaders/")
    file(MAKE_DIRECTORY "${SHADER_OUT_DIRECTORY}")

    foreach(SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
        cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
        cmake_path(GET SHADER_SOURCE FILENAME SHADER_NAME)

        set(SHADER_OUT_PATH "${SHADER_OUT_DIRECTORY}/${SHADER_NAME}.spv")

        list(APPEND SHADER_COMMANDS COMMAND Vulkan::glslc "${SHADER_SOURCE}" -o "${SHADER_OUT_PATH}")
        list(APPEND COMPILED_SHADERS "${SHADER_OUT_PATH}")
    endforeach()

    # PRE_BUILD only works in visual studio, thanks
    # add_custom_command(
    #     TARGET ${TARGET_NAME}
    #     PRE_BUILD
    #     ${SHADER_COMMANDS}
    #     COMMENT "Compiling Shaders"
    #     SOURCES ${SHADER_SOURCE_FILES}
    #     BYPRODUCTS ${COMPILED_SHADERS}
    # )

    SET(SHADER_TARGET_NAME "${TARGET_NAME}_shaders")

    add_custom_target(
        ${SHADER_TARGET_NAME}
        ${SHADER_COMMANDS}
        COMMENT "Compiling Shaders"
        SOURCES ${SHADER_SOURCE_FILES}
        BYPRODUCTS ${COMPILED_SHADERS}
    )

    add_dependencies(${TARGET_NAME} ${SHADER_TARGET_NAME})

    target_compile_options(${TARGET_NAME} PRIVATE "--embed-dir=${CMAKE_BINARY_DIR}/compiled-shaders/")
endfunction()
