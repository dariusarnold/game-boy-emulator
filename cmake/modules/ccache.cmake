# Find ccache executable

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message(STATUS "Enabling ccache")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
else()
    message(WARNING "USE_CCACHE is on but could not be enabled because ccache executable not found!")
endif()
