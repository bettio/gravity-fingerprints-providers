project(gravity-fingerprints-providers CXX)

cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
find_package(CommonModules REQUIRED)

#################################################################################################
# Modify these variables to what suits the application best. Remember, no dashes in the version!
set(GRAVITY_FINGERPRINTS_PROVIDERS_MAJOR_VERSION 0)
set(GRAVITY_FINGERPRINTS_PROVIDERS_MINOR_VERSION 91)
set(GRAVITY_FINGERPRINTS_PROVIDERS_RELEASE_VERSION 91)
option(GRAVITY_FINGERPRINTS_PROVIDERS_DEVELOPMENT_RELEASE "Must be ON unless we're releasing" ON)

option(ENABLE_WERROR "Enables WError. Always enable when developing, and disable when releasing." ON)

set(GRAVITY_FINGERPRINTS_PROVIDERS_DBUS_SYSTEM_ACTIVATION_DIR ${CMAKE_INSTALL_PREFIX}/share/dbus-1/system-services CACHE PATH "Location of DBus activatable system services.")

#################################################################################################

set(GRAVITY_FINGERPRINTS_PROVIDERS_VERSION ${GRAVITY_FINGERPRINTS_PROVIDERS_MAJOR_VERSION}.${GRAVITY_FINGERPRINTS_PROVIDERS_MINOR_VERSION}.${GRAVITY_FINGERPRINTS_PROVIDERS_RELEASE_VERSION})
if (GRAVITY_FINGERPRINTS_PROVIDERS_DEVELOPMENT_RELEASE)
    include(GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    STRING(SUBSTRING ${GIT_SHA1} 0 8 GIT_SHA1_SMALL)
    set(GRAVITY_FINGERPRINTS_PROVIDERS_VERSION_STRING ${GRAVITY_FINGERPRINTS_PROVIDERS_MAJOR_VERSION}.${GRAVITY_FINGERPRINTS_PROVIDERS_MINOR_VERSION}.${GRAVITY_FINGERPRINTS_PROVIDERS_RELEASE_VERSION}+git${GIT_SHA1_SMALL})
else (GRAVITY_FINGERPRINTS_PROVIDERS_DEVELOPMENT_RELEASE)
    set(GRAVITY_FINGERPRINTS_PROVIDERS_VERSION_STRING ${GRAVITY_FINGERPRINTS_PROVIDERS_MAJOR_VERSION}.${GRAVITY_FINGERPRINTS_PROVIDERS_MINOR_VERSION}.${GRAVITY_FINGERPRINTS_PROVIDERS_RELEASE_VERSION})
endif (GRAVITY_FINGERPRINTS_PROVIDERS_DEVELOPMENT_RELEASE)

find_package(Qt5 COMPONENTS Core Concurrent DBus Network REQUIRED)
find_package(HemeraQt5SDK 0.90.0 COMPONENTS Core REQUIRED)
find_package(Gravity 0.90.0 COMPONENTS Fingerprints)
# hemera-fingerprints requires this to generate hardware id
find_package(UDev REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${UDEV_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR}/include)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

include(EnableCoverageReport)
include(EnableSlocCount)
include(GenerateCppcheck)
include(GitArchive)
include(CompilerWarnings)
include(TestCXXAcceptsFlag)

# Warnings. Enable WError, this component is critical.
if(${ENABLE_WERROR})
    set(RET_W 1)
else(${ENABLE_WERROR})
    set(RET_W 0)
endif(${ENABLE_WERROR})

set(desired
    all
    extra
    sign-compare
    pointer-arith
    format-security
    init-self
    non-virtual-dtor)
set(undesired
    missing-field-initializers
    unused-parameter)
compiler_warnings(CMAKE_CXX_FLAGS_WARNINGS cxx ${RET_W} "${desired}" "${undesired}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_WARNINGS}")

# C++11
if (CMAKE_COMPILER_IS_GNUCXX)
   execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
        message(STATUS "C++11 activated.")
        add_definitions("-std=gnu++11")
   elseif (GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3)
        message(WARNING "C++0x activated. If you get any errors update to a compiler which fully supports C++11")
        add_definitions("-std=gnu++0x")
   else ()
        message(FATAL_ERROR "C++11 needed. Therefore a gcc compiler with a version higher than 4.3 is needed.")
   endif()
else (CMAKE_COMPILER_IS_GNUCXX)
   add_definitions("-std=c++0x")
endif (CMAKE_COMPILER_IS_GNUCXX)

# When building Gravity, we want every symbol to be resolvable immediately even if we're building a shared library.
add_definitions(-Wl,-z,defs)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z defs")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -z defs")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -z defs")

# We want explicit literals all over the place, and fast concat
add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_USE_QSTRINGBUILDER)

# Enable compilation of tests and examples by default only if we're not in a submodule
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    option(ENABLE_GRAVITY_FINGERPRINTS_PROVIDERS_EXAMPLES "Enable compilation of examples" ON)
    option(ENABLE_GRAVITY_FINGERPRINTS_PROVIDERS_TESTS "Enable compilation of tests" OFF)
else (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    option(ENABLE_GRAVITY_FINGERPRINTS_PROVIDERS_EXAMPLES "Enable compilation of examples" OFF)
    option(ENABLE_GRAVITY_FINGERPRINTS_PROVIDERS_TESTS "Enable compilation of tests" OFF)
endif (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})

option(ENABLE_GRAVITY_FINGERPRINTS_PROVIDERS_COVERAGE "Enable compiler coverage" OFF)

# Definitions
add_definitions(-DGRAVITY_FINGERPRINTS_PROVIDERS_VERSION="${GRAVITY_FINGERPRINTS_PROVIDERS_VERSION_STRING}")

# Config file
#configure_file(gravityconfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/gravityconfig.h" @ONLY)

if (ENABLE_GRAVITY_FINGERPRINTS_PROVIDERS_TESTS)
    hemera_setup_test_targets("Gravity Fingerprints Providers" ${ENABLE_GRAVITY_FINGERPRINTS_PROVIDERS_COVERAGE} 60)
    add_subdirectory(tests)
endif (ENABLE_GRAVITY_FINGERPRINTS_PROVIDERS_TESTS)

# providers
add_subdirectory(imx)

if (ENABLE_GRAVITY_FINGERPRINTS_PROVIDERS_EXAMPLES)
#     add_subdirectory(testApp)
endif (ENABLE_GRAVITY_FINGERPRINTS_PROVIDERS_EXAMPLES)

# Add these targets only if we are in the root dir
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    # --- sloccount ---

    enable_sloccount(FOLDERS src testApp)

    # --- cppcheck ---

    generate_cppcheck(SOURCES src testApp
                      ENABLE_IDS all
                      INLINE_SUPPRESSION)
endif (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})

# Archive
add_make_dist_target(gravity ${GRAVITY_FINGERPRINTS_PROVIDERS_VERSION_STRING})

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
