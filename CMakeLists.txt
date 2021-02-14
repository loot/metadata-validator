cmake_minimum_required (VERSION 2.8.12.1)
cmake_policy(SET CMP0015 NEW)
project (metadata-validator)
include(ExternalProject)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

##############################
# Get Build Revision
##############################

find_package(Git)

if (GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  OUTPUT_VARIABLE GIT_COMMIT_STRING
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    SET (GIT_COMMIT_STRING "unknown")
endif()

message(STATUS "Git revision: ${GIT_COMMIT_STRING}")

# Write to file.
configure_file("${CMAKE_SOURCE_DIR}/src/version.cpp.in" "${CMAKE_BINARY_DIR}/generated/version.cpp" @ONLY)
include_directories("${CMAKE_BINARY_DIR}/generated")

##############################
# LOOT API
##############################

if (NOT DEFINED LIBLOOT_URL)
    if (CMAKE_SYSTEM_NAME MATCHES "Windows")
        if (NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
            set(LIBLOOT_URL "https://github.com/loot/libloot/releases/download/0.16.2/libloot-0.16.2-0-gc74c16b_0.16.2-win32.7z")
        else()
            set(LIBLOOT_URL "https://github.com/loot/libloot/releases/download/0.16.2/libloot-0.16.2-0-gc74c16b_0.16.2-win64.7z")
        endif()
    else()
        set(LIBLOOT_URL "https://github.com/loot/libloot/releases/download/0.16.2/libloot-0.16.2-0-gc74c16b_0.16.2-Linux.tar.xz")
    endif()
endif()

ExternalProject_Add(libloot
                    PREFIX "external"
                    URL ${LIBLOOT_URL}
                    CONFIGURE_COMMAND ""
                    BUILD_COMMAND ""
                    INSTALL_COMMAND "")
ExternalProject_Get_Property(libloot SOURCE_DIR)
set(LIBLOOT_EXTRACTED_PATH ${SOURCE_DIR})

include_directories("${LIBLOOT_EXTRACTED_PATH}/include")
link_directories(${LIBLOOT_EXTRACTED_PATH})

set(LIBLOOT_STATIC_LIBRARY "${CMAKE_STATIC_LIBRARY_PREFIX}loot${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(LIBLOOT_SHARED_LIBRARY "${CMAKE_SHARED_LIBRARY_PREFIX}loot${CMAKE_SHARED_LIBRARY_SUFFIX}")

##############################
# General Settings
##############################

set(VALIDATOR_HEADERS "${CMAKE_SOURCE_DIR}/src/version.h")

set(VALIDATOR_SOURCE  "${CMAKE_BINARY_DIR}/generated/version.cpp"
                      "${CMAKE_SOURCE_DIR}/src/main.cpp"
                      "${CMAKE_SOURCE_DIR}/src/resource.rc")

include_directories("${CMAKE_SOURCE_DIR}/src")

##############################
# System-Specific Settings
##############################

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions (-DUNICODE -D_UNICODE)
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};.")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
endif()

##############################
# Define Targets
##############################

# Build validator.
add_executable       (metadata_validator ${VALIDATOR_SOURCE} ${VALIDATOR_HEADERS})
add_dependencies     (metadata_validator libloot)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(metadata_validator PRIVATE ${LIBLOOT_STATIC_LIBRARY})
else()
    target_link_libraries(metadata_validator PRIVATE ${LIBLOOT_SHARED_LIBRARY} pthread http_parser ssh2 stdc++fs)
endif()

# Copy the API binary to the build directory.
add_custom_command(TARGET metadata_validator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LIBLOOT_EXTRACTED_PATH}/${LIBLOOT_SHARED_LIBRARY}"
        "$<TARGET_FILE_DIR:metadata_validator>/${LIBLOOT_SHARED_LIBRARY}")

########################################
# Install
########################################

install(TARGETS metadata_validator
        DESTINATION ".")

install(FILES "${LIBLOOT_EXTRACTED_PATH}/${LIBLOOT_SHARED_LIBRARY}"
        "${CMAKE_SOURCE_DIR}/README.md"
        DESTINATION ".")

########################################
# CPack
########################################

IF (NOT DEFINED CPACK_PACKAGE_VERSION)
    IF (GIT_FOUND)
        execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --long --always --abbrev=7
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_DESCRIBE_STRING
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    ELSEIF (DEFINED ENV{TRAVIS_BRANCH})
        SET (GIT_DESCRIBE_STRING "${GIT_DESCRIBE_STRING}_$ENV{TRAVIS_BRANCH}")
    ELSE ()
        SET (GIT_DESCRIBE_STRING "unknown-version")
    ENDIF ()

    set(CPACK_PACKAGE_VERSION ${GIT_DESCRIBE_STRING})
ENDIF()

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CPACK_GENERATOR "7Z")
else()
    set(CPACK_GENERATOR "TXZ")
endif()

set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package")

include(CPack)
