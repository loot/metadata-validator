cmake_minimum_required(VERSION 3.10)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
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
# External projects
##############################

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(ICU REQUIRED COMPONENTS data uc)
    find_package(TBB REQUIRED)
    find_package(libloot REQUIRED CONFIG)
else()
    find_package(libloot CONFIG)
endif()

if(libloot_FOUND)
    get_target_property(LIBLOOT_INCLUDE_DIRS libloot::loot INTERFACE_INCLUDE_DIRECTORIES)
    set(LIBLOOT_SHARED_LIBRARY $<TARGET_LINKER_FILE:libloot::loot>)
else()
    if(NOT DEFINED LIBLOOT_URL)
        if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
            set(LIBLOOT_URL "https://github.com/loot/libloot/releases/download/0.28.0/libloot-0.28.0-win32.7z")
            set(LIBLOOT_HASH "SHA256=b9e5bde326da7c1a3324493302d8a6aa894711bd37f398af0ffc96b847603d14")
        else()
            set(LIBLOOT_URL "https://github.com/loot/libloot/releases/download/0.28.0/libloot-0.28.0-win64.7z")
            set(LIBLOOT_HASH "SHA256=9cee812c17538fa0e3cbf376a2afc92431c42b606a02c9f7395503cd64327b72")
        endif()
    else()
        file(TO_CMAKE_PATH "${LIBLOOT_URL}" LIBLOOT_URL)
    endif()

    ExternalProject_Add(libloot
        PREFIX "external"
        URL ${LIBLOOT_URL}
        URL_HASH ${LIBLOOT_HASH}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND "")
    ExternalProject_Get_Property(libloot SOURCE_DIR)
    set(LIBLOOT_INCLUDE_DIRS "${SOURCE_DIR}/include")
    set(LIBLOOT_STATIC_LIBRARY "${SOURCE_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}loot${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(LIBLOOT_SHARED_LIBRARY "${SOURCE_DIR}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}loot${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

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
add_executable(metadata_validator ${VALIDATOR_SOURCE} ${VALIDATOR_HEADERS})

if(libloot_FOUND)
    target_link_libraries(metadata_validator PRIVATE libloot::loot)
else()
    add_dependencies(metadata_validator libloot)

    target_include_directories(metadata_validator SYSTEM PRIVATE
        ${LIBLOOT_INCLUDE_DIRS})

    target_link_libraries(metadata_validator PRIVATE ${LIBLOOT_STATIC_LIBRARY})
endif()

target_include_directories(metadata_validator PRIVATE "${CMAKE_SOURCE_DIR}/src")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(metadata_validator PRIVATE UNICODE _UNICODE)
    target_compile_options(metadata_validator PRIVATE "/permissive-" "/W4")
else()
    target_link_libraries(metadata_validator PRIVATE pthread stdc++fs ICU::data ICU::uc TBB::tbb)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set_target_properties(metadata_validator
        PROPERTIES
            INSTALL_RPATH "${CMAKE_INSTALL_RPATH};."
            BUILD_WITH_INSTALL_RPATH ON)

    target_compile_options(metadata_validator PRIVATE "-Wall" "-Wextra")
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
