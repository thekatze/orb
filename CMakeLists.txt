cmake_minimum_required(VERSION 3.28)

SET(CMAKE_C_STANDARD 23)

SET(PEDANTIC "-Wpedantic")

SET(DEBUG_C_FLAGS)

if (APPLE)
    # when debugging platform_mac.m, comment out these lines and remove the -Werror compiler flag.
    # xcode only works with clang that doesnt support c23 yet, so we get extra warnings there
    SET(CMAKE_C_COMPILER "/opt/homebrew/opt/gcc/bin/gcc-14")
    SET(DEBUG_C_FLAGS "-ggdb -fsanitize=leak")
elseif(LINUX)
    # -fsanitize=address does not work on mac
    SET(DEBUG_C_FLAGS "-g3") # -fsanitize=leak -fsanitize=address")
else()
    # -fsanitize=leak does not work on windows
    SET(PEDANTIC "-Wauto-decl-extensions -Wbit-int-extension -Wc++11-extra-semi -Wc++11-long-long -Wc++14-attribute-extensions -Wc++14-binary-literal -Wc++17-attribute-extensions -Wc++20-attribute-extensions -Wc++20-designator -Wc11-extensions -Wcomplex-component-init -Wdelimited-escape-sequence-extension -Wdollar-in-identifier-extension -Wembedded-directive -Wempty-translation-unit -Wfixed-enum-extension -Wflexible-array-extensions -Wfuture-attribute-extensions -Wgnu-anonymous-struct -Wgnu-auto-type -Wgnu-case-range -Wgnu-complex-integer -Wgnu-compound-literal-initializer -Wgnu-conditional-omitted-operand -Wgnu-empty-struct -Wgnu-flexible-array-initializer -Wgnu-flexible-array-union-member -Wgnu-imaginary-constant -Wgnu-include-next -Wgnu-label-as-value -Wgnu-line-marker -Wgnu-null-pointer-arithmetic -Wgnu-pointer-arith -Wgnu-redeclared-enum -Wgnu-statement-expression -Wgnu-union-cast -Wgnu-zero-line-directive -Wimport-preprocessor-directive-pedantic -Winvalid-utf8 -Wkeyword-macro -Wlanguage-extension-token -Wlong-long -Wmicrosoft-charize -Wmicrosoft-comment-paste -Wmicrosoft-cpp-macro -Wmicrosoft-end-of-file -Wmicrosoft-enum-value -Wmicrosoft-fixed-enum -Wmicrosoft-flexible-array -Wmicrosoft-redeclare-static -Wnested-anon-types -Wnullability-extension -Woverlength-strings -Wretained-language-linkage -Wundefined-internal-type -Wzero-length-array")
    SET(DEBUG_C_FLAGS "-ggdb -fsanitize=address")
endif()

SET(RELEASE_C_FLAGS)

if (CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building orb in release mode")
    SET(ORB_RELEASE TRUE)
    SET(RELEASE_C_FLAGS "-O2")
else()
    message("Building orb in debug mode")
    SET(ORB_RELEASE FALSE)
endif()

SET(CMAKE_C_FLAGS "${DEBUG_C_FLAGS} ${RELEASE_C_FLAGS} -Werror -Wall ${PEDANTIC} -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch -Wconversion -Wunreachable-code")

# for clangd
SET(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(
    orb
    VERSION 0.1.0
    LANGUAGES C
)

include("./cmake/add_shaders.cmake")

add_subdirectory(engine)
add_subdirectory(example)
add_subdirectory(tests)

