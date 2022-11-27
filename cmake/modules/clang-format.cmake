# A CMake script to find all source files and setup clang-format targets for them

# Find all source files
set(CLANG_FORMAT_CXX_FILE_EXTENSIONS ${CLANG_FORMAT_CXX_FILE_EXTENSIONS} *.cpp *.h *.cxx *.hxx *.hpp *.cc *.ipp)
file(GLOB_RECURSE ALL_SOURCE_FILES ${CLANG_FORMAT_CXX_FILE_EXTENSIONS})

# Don't include some common build folders
set(CLANG_FORMAT_EXCLUDE_PATTERNS ${CLANG_FORMAT_EXCLUDE_PATTERNS} "/CMakeFiles/" "cmake")

# get all project files file
foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
    foreach (EXCLUDE_PATTERN ${CLANG_FORMAT_EXCLUDE_PATTERNS})
        string(FIND ${SOURCE_FILE} ${EXCLUDE_PATTERN} EXCLUDE_FOUND)
        if (NOT ${EXCLUDE_FOUND} EQUAL -1)
            list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
        endif ()
    endforeach ()
endforeach ()

add_custom_target(clang-format
        COMMENT "Running clang-format to change files"
        COMMAND ${ClangFormat_EXECUTABLE}
        --style=file
        -i
        ${ALL_SOURCE_FILES}
        )

add_custom_target(clang-format-check
        COMMENT "Checking clang-format changes"
        COMMAND ${ClangFormat_EXECUTABLE}
        --style=file
        --dry-run
        --Werror
        ${ALL_SOURCE_FILES}
        )

# Get the path to this file
get_filename_component(_clangcheckpath clang-format.cmake PATH)
# have at least one here by default
set(CHANGED_FILE_EXTENSIONS ".cpp")
foreach (EXTENSION ${CLANG_FORMAT_CXX_FILE_EXTENSIONS})
    set(CHANGED_FILE_EXTENSIONS "${CHANGED_FILE_EXTENSIONS},${EXTENSION}")
endforeach ()

set(EXCLUDE_PATTERN_ARGS)
foreach (EXCLUDE_PATTERN ${CLANG_FORMAT_EXCLUDE_PATTERNS})
    list(APPEND EXCLUDE_PATTERN_ARGS "--exclude=${EXCLUDE_PATTERN}")
endforeach ()