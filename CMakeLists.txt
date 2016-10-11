cmake_minimum_required(VERSION 2.8)

set( LL_MODULE "crypto" )
project( "ll_${LL_MODULE}" )

# -------------------------------------------------------------------------------------------------
# CMake setup
# -------------------------------------------------------------------------------------------------

set( LL_PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR} )
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/support/cmake;${CMAKE_MODULE_PATH} )
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cfg;${CMAKE_MODULE_PATH} )

include( config )
include( build_settings )
include( utilities )


# -------------------------------------------------------------------------------------------------
# Library
# -------------------------------------------------------------------------------------------------

# include paths
include_directories( "${CMAKE_CURRENT_LIST_DIR}/include" )

#sources

SOURCE_GROUP( include  "include/" )
SOURCE_GROUP( source  "src/" )

set( SRC_FILE_LIST "" )

add_ll_source( ${LL_MODULE} SRC_FILE_LIST "include/crypto/password.h" )
add_ll_source( ${LL_MODULE} SRC_FILE_LIST "src/internal_utils.h" )

add_ll_source( ${LL_MODULE} SRC_FILE_LIST "src/hash.cpp" HAS_PUBLIC_HEADER )
add_ll_source( ${LL_MODULE} SRC_FILE_LIST "src/exception.cpp" HAS_PUBLIC_HEADER )

if(WIN32)
  add_ll_source( ${LL_MODULE} SRC_FILE_LIST "src/hash_impl_win.cpp" )
  add_ll_source( ${LL_MODULE} SRC_FILE_LIST "src/password_impl_win.cpp" )
else()
  add_ll_source( ${LL_MODULE} SRC_FILE_LIST "src/hash_impl_posix.cpp" )
  add_ll_source( ${LL_MODULE} SRC_FILE_LIST "src/password_impl_posix.cpp" )
endif()
  
add_library( "ll_${LL_MODULE}${LL_VERSION_STRING}${LL_ARCHITECTURE_POSTFIX}" STATIC ${SRC_FILE_LIST} )

#if( ${LL_BUILD_ARCHITECTURE} MATCHES "x32" )
#  set_target_properties( "ll_${LL_MODULE}${LL_ARCHITECTURE_POSTFIX}" PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
#endif() 

# -------------------------------------------------------------------------------------------------
# Tests
# -------------------------------------------------------------------------------------------------

set( TEST_EXE_NAME "ll_${LL_MODULE}_test${LL_ARCHITECTURE_POSTFIX}" )
set( TESTCASE_DIR "tests/testcases" )
set( HELPERS_DIR "tests/helpers" )

# paths
include_directories( "${CATCH_INCLUDE_DIR}" )
include_directories( "${BOOST_INCLUDE_DIR}" )

link_directories( "${BOOST_LIB_DIR}" )


# sources
SOURCE_GROUP( src\\helpers  "tests/helpers/" )
SOURCE_GROUP( src\\testcases  "tests/testcases/" )
SOURCE_GROUP( src "tests/" )
SOURCE_GROUP( helpers "helpers/" )

set( TEST_SRC_LIST "" )

list( APPEND TEST_SRC_LIST "${TESTCASE_DIR}/hash.test.cpp" )
list( APPEND TEST_SRC_LIST "${TESTCASE_DIR}/password.test.cpp" )


list( APPEND TEST_SRC_LIST "tests/main.cpp" )


add_executable( ${TEST_EXE_NAME} ${TEST_SRC_LIST} )
#if( ${LL_BUILD_ARCHITECTURE} MATCHES "x32" )
#  set_target_properties( ${TEST_EXE_NAME} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
#endif() 


# link to the module(s)
add_ll_module( ${TEST_EXE_NAME} ${LL_MODULE} )


# link to externals
if( WIN32 )

  target_link_libraries( ${TEST_EXE_NAME} "bcrypt" )  

else()
  
  if( APPLE )

    find_library(COCOA_FRAMEWORK Cocoa)
    if (NOT COCOA_FRAMEWORK)
      message( FATAL_ERROR "Cocoa Framework not found" )
    endif()

    find_library(SECURITY_FRAMEWORK Security)
    if (NOT SECURITY_FRAMEWORK)
      message( FATAL_ERROR "Security Framework not found" )
    endif()

    target_link_libraries( ${TEST_EXE_NAME} ${COCOA_FRAMEWORK} ${SECURITY_FRAMEWORK} )

  else()

    target_link_libraries( ${TEST_EXE_NAME} ssl crypto )

  endif()

  target_link_libraries( ${TEST_EXE_NAME} pthread )
  target_link_libraries( ${TEST_EXE_NAME} boost_filesystem boost_system )  

  
endif()


# post-build step (copy test data)
add_custom_command( TARGET ${TEST_EXE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/tests/data 
    $<TARGET_FILE_DIR:${TEST_EXE_NAME}>/data
)