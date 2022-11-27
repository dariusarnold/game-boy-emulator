# Find Clang format
# The module defines the following variables
#
# ``ClangFormat_FOUND`` True if clang-format executable was found.
# ``ClangFormat_EXECUTABLE`` Path to clang-format executable.
#
# The behaviour of this module is influenced by the following variables
#
# ``CLANG_FORMAT_EXE_NAME`` Name of executable to find (default clang-format).
# ``CLANG_FORMAT_ROOT_DIR`` Path to search for executable name in.
#
if (NOT CLANG_FORMAT_EXE_NAME)
    set(CLANG_FORMAT_EXE_NAME clang-format)
endif ()

# if custom path check there first
if (CLANG_FORMAT_ROOT_DIR)
    find_program(ClangFormat_EXECUTABLE
            NAMES
            ${CLANG_FORMAT_EXE_NAME}
            PATHS
            "${CLANG_FORMAT_ROOT_DIR}"
            NO_DEFAULT_PATH)
else ()
    find_program(ClangFormat_EXECUTABLE NAMES ${CLANG_FORMAT_EXE_NAME})
endif ()

# Print an error message when the executable was not found
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
        ClangFormat
        DEFAULT_MSG
        ClangFormat_EXECUTABLE)

mark_as_advanced(ClangFormat_EXECUTABLE)

if (ClangFormat_FOUND)
    # A CMake script to find all source files and setup clang-format targets for them
    include(clang-format)
else ()
    message(STATUS "clang-format not found. Not setting up format targets")
endif ()