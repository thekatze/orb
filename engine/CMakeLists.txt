set(orb_INCLUDE_DIR "${CMAKE_HOME_DIRECTORY}/engine/src/" PARENT_SCOPE)

SET(MAC_PLATFORM_SRC "")
if (APPLE) 
    enable_language(OBJC)
    SET(MAC_PLATFORM_SRC "./src/orb/platform/platform_mac.m")
endif()

add_library(orb 
    ./src/orb/core/logger.c
    ./src/orb/core/application.c

    ./src/orb/platform/platform_linux.c
    "${MAC_PLATFORM_SRC}"
)

target_compile_definitions(orb PUBLIC ORB_EXPORT ORB_RELEASE=0 ORB_ASSERTIONS_ENABLED)

if(LINUX) 
    find_package(X11 REQUIRED)

    target_include_directories(orb PRIVATE "${X11_INCLUDE_DIR}")
    target_link_libraries(orb PRIVATE "${X11_LIBRARIES}" X11::xcb X11::X11_xcb)
elseif(APPLE)
    target_link_libraries(orb PRIVATE "-framework Foundation -framework AppKit -framework QuartzCore")
endif()

