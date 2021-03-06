########################################################
#  
#  This is a CMake configuration file.
#  To use it you need CMake which can be 
#  downloaded from here: 
#    http://www.cmake.org/cmake/resources/software.html
#
#########################################################

cmake_minimum_required( VERSION 2.8 ) 
project( ApkFactory ) 

set( CMAKE_DEBUG_POSTFIX "d" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( PACKAGE_DIRECTORY ${PROJECT_BINARY_DIR}/package )
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CmakeExtras" )

set( TOOLS_MAJOR_VERSION 3 )
set( TOOLS_MINOR_VERSION 0 )
set( TOOLS_REVISION_VERSION 4 )
set( TOOLS_FULL_VERSION ${TOOLS_MAJOR_VERSION}.${TOOLS_MINOR_VERSION}.${TOOLS_REVISION_VERSION} )

# Apple users building with XCode notes:
# If building with XCode you need to set the target and location
# of the SDK. If you are building with the Command Line Tools
# you do not need to specify the follwing options.
# It is recommened to set them when running cmake using -D
# rather than editing this file directly.
# CMAKE_OSX_DEPLOYMENT_TARGET "10.8"
# CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX10.8.sdk"


# Check if platform is 64 bit
if( NOT APPLE )
   if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
     set( 64_BIT_PLATFORM 0 )
   else()
     set( 64_BIT_PLATFORM 1 )
   endif()
endif()

# Profiler configuration for GCC
if( CMAKE_COMPILER_IS_GNUCXX  )
    set( CMAKE_BUILD_TYPE profiling )
    set( CMAKE_CXX_FLAGS_PROFILING "-O2 -DNDEBUG -pg -g -fno-omit-frame-pointer -fno-inline-functions -fno-inline-functions-called-once -fno-optimize-sibling-calls" )

    # After creating the new build type,
    # we clear the default back to empty
    set( CMAKE_BUILD_TYPE  )
endif()

# This disables the building of the test runner app for FlightCrew
set( NO_TEST_EXE 1 )


if( FORCE_BUNDLED_COPIES OR NOT PCRE_FOUND OR APPLE )
    add_subdirectory( Src/pcre )
    set( PCRE_LIBRARIES pcre )
    set( PCRE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Src/pcre ${CMAKE_BINARY_DIR}/Src/pcre )
endif()

add_subdirectory( src/ApkFactory )
