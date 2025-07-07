set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(rb)

if(IS_TOP_LEVEL_PROJECT)
    # build app
    set(APP rb-main)
    add_executable(${APP} main.cpp)
    target_link_libraries(${APP} PUBLIC Rb::Rb)
    use_sanitizers(${APP})
endif()
