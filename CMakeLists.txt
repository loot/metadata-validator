cmake_minimum_required (VERSION 2.8.12.1)
cmake_policy(SET CMP0015 NEW)
project (metadata-validator)
include(ExternalProject)

##############################
# LOOT API
##############################

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(LOOT_API_URL "https://github.com/loot/loot/releases/download/0.10.3/loot-api_0.10.3-0-g0fcf788_dev_Win32.7z")
else()
    set(LOOT_API_URL "https://github.com/loot/loot/releases/download/0.10.3/loot-api_0.10.3-0-g0fcf788_0.10.3.tar.xz")
endif()

ExternalProject_Add(loot_api
                    PREFIX "external"
                    URL ${LOOT_API_URL}
                    CONFIGURE_COMMAND ""
                    BUILD_COMMAND ""
                    INSTALL_COMMAND "")
ExternalProject_Get_Property(loot_api SOURCE_DIR)
set(LOOT_API_EXTRACTED_PATH ${SOURCE_DIR})

include_directories("${LOOT_API_EXTRACTED_PATH}/include")
link_directories(${LOOT_API_EXTRACTED_PATH})

set(LOOT_API_STATIC_LIBRARY "${CMAKE_STATIC_LIBRARY_PREFIX}loot_api${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(LOOT_API_SHARED_LIBRARY "${CMAKE_SHARED_LIBRARY_PREFIX}loot_api${CMAKE_SHARED_LIBRARY_SUFFIX}")

##############################
# General Settings
##############################

set(VALIDATOR_SOURCE "${CMAKE_SOURCE_DIR}/src/main.cpp")

##############################
# System-Specific Settings
##############################

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions (-DUNICODE -D_UNICODE -DLIBLO_STATIC)
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};.")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -std=c++14")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++14")
endif()

##############################
# Define Targets
##############################

# Build validator.
add_executable       (metadata_validator ${VALIDATOR_SOURCE})
add_dependencies     (metadata_validator loot_api)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(metadata_validator PRIVATE ${LOOT_API_STATIC_LIBRARY})
else()
    target_link_libraries(metadata_validator PRIVATE ${LOOT_API_SHARED_LIBRARY} "stdc++fs")
endif()

########################################
# Install
########################################

install(TARGETS metadata_validator
        DESTINATION ".")

install(FILES "${LOOT_API_EXTRACTED_PATH}/${LOOT_API_SHARED_LIBRARY}"
        "${CMAKE_SOURCE_DIR}/README.md"
        DESTINATION ".")

########################################
# CPack
########################################

# Get version info using Git if available
find_package(Git)

IF (GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --long --always
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  OUTPUT_VARIABLE GIT_DESCRIBE_STRING
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

    IF (DEFINED ENV{APPVEYOR_REPO_BRANCH})
        set(GIT_DESCRIBE_STRING "${GIT_DESCRIBE_STRING}_$ENV{APPVEYOR_REPO_BRANCH}")
    ELSEIF (DEFINED ENV{TRAVIS_BRANCH})
        set(GIT_DESCRIBE_STRING "${GIT_DESCRIBE_STRING}_$ENV{TRAVIS_BRANCH}")
    ENDIF()
ELSE()
    SET (GIT_DESCRIBE_STRING "unknown-version")
ENDIF ()

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CPACK_GENERATOR "7Z")
else()
    set(CPACK_GENERATOR "TXZ")
endif()

set(CPACK_PACKAGE_VERSION ${GIT_DESCRIBE_STRING})
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/package")

include(CPack)
