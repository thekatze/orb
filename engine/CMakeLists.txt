set(orb_INCLUDE_DIR "${CMAKE_HOME_DIRECTORY}/engine/src/" PARENT_SCOPE)

add_library(orb 
    ./src/orb/core/logger.c
    ./src/orb/core/application.c

    ./src/orb/platform/platform_mac.c 
    ./src/orb/platform/platform_linux.c
)

target_compile_definitions(orb PUBLIC ORB_EXPORT ORB_RELEASE=0 ORB_ASSERTIONS_ENABLED)

if(LINUX) 
    find_package(X11 REQUIRED)

    target_include_directories(orb PRIVATE "${X11_INCLUDE_DIR}")
    target_link_libraries(orb PRIVATE "${X11_LIBRARIES}" X11::xcb X11::X11_xcb)
endif()

