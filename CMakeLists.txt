cmake_minimum_required(VERSION 3.4)
cmake_policy(SET CMP0015 NEW)
project(metadata-validator)
include(ExternalProject)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

##############################
# Get Build Revision
##############################

find_package(Git)

if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_STRING
        OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    set(GIT_COMMIT_STRING "unknown")
endif()

message(STATUS "Git revision: ${GIT_COMMIT_STRING}")

# Write to file.
configure_file("${CMAKE_SOURCE_DIR}/src/version.cpp.in"
    "${CMAKE_BINARY_DIR}/generated/version.cpp"
    @ONLY)

##############################
# libloot
##############################

if(NOT DEFINED LIBLOOT_URL)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
            set(LIBLOOT_URL "https://github.com/loot/libloot/releases/download/0.17.0/libloot-0.17.0-0-ge8414a1_0.17.0-win32.7z")
        else()
            set(LIBLOOT_URL "https://github.com/loot/libloot/releases/download/0.17.0/libloot-0.17.0-0-ge8414a1_0.17.0-win64.7z")
        endif()
    else()
        set(LIBLOOT_URL "https://github.com/loot/libloot/releases/download/0.17.0/libloot-0.17.0-0-ge8414a1_0.17.0-Linux.tar.xz")
    endif()
endif()

ExternalProject_Add(libloot
    PREFIX "external"
    URL ${LIBLOOT_URL}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "")
ExternalProject_Get_Property(libloot SOURCE_DIR)
set(LIBLOOT_INCLUDE_DIRS "${SOURCE_DIR}/include")
set(LIBLOOT_STATIC_LIBRARY "${SOURCE_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}loot${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(LIBLOOT_SHARED_LIBRARY "${SOURCE_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}loot${CMAKE_SHARED_LIBRARY_SUFFIX}")

##############################
# General Settings
##############################

set(VALIDATOR_HEADERS "${CMAKE_SOURCE_DIR}/src/version.h")

set(VALIDATOR_SOURCE "${CMAKE_BINARY_DIR}/generated/version.cpp"
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/resource.rc")

##############################
# Define Targets
##############################

# Build validator.
add_executable       (metadata_validator ${VALIDATOR_SOURCE} ${VALIDATOR_HEADERS})
add_dependencies     (metadata_validator libloot)

target_include_directories(metadata_validator PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    ${LIBLOOT_INCLUDE_DIRS})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(metadata_validator PRIVATE UNICODE _UNICODE)
    target_compile_options(metadata_validator PRIVATE "/permissive-" "/W4")

    target_link_libraries(metadata_validator PRIVATE ${LIBLOOT_STATIC_LIBRARY})
else()
    target_link_libraries(metadata_validator PRIVATE ${LIBLOOT_SHARED_LIBRARY} pthread http_parser ssh2 stdc++fs)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set_target_properties(metadata_validator
        PROPERTIES
            INSTALL_RPATH "${CMAKE_INSTALL_RPATH};."
            BUILD_WITH_INSTALL_RPATH ON)
endif()

# Copy the API binary to the build directory.
get_filename_component(LIBLOOT_SHARED_LIBRARY_FILENAME ${LIBLOOT_SHARED_LIBRARY} NAME)
add_custom_command(TARGET metadata_validator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LIBLOOT_SHARED_LIBRARY}
        "$<TARGET_FILE_DIR:metadata_validator>/${LIBLOOT_SHARED_LIBRARY_FILENAME}")

########################################
# Install
########################################

install(TARGETS metadata_validator
    DESTINATION ".")

install(FILES ${LIBLOOT_SHARED_LIBRARY}
    "${CMAKE_SOURCE_DIR}/README.md"
    DESTINATION ".")

########################################
# CPack
########################################

if(NOT DEFINED CPACK_PACKAGE_VERSION)
    if(GIT_FOUND)
        execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --long --always --abbrev=7
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_DESCRIBE_STRING
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
        set(GIT_DESCRIBE_STRING "unknown-version")
    endif()

    set(CPACK_PACKAGE_VERSION ${GIT_DESCRIBE_STRING})
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_GENERATOR "7Z")
else()
    set(CPACK_GENERATOR "TXZ")
endif()

set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package")

include(CPack)
