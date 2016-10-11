cmake_minimum_required(VERSION 2.8)

# -------------------------------------------------------------------------------------------------
# add_ll_source
# -------------------------------------------------------------------------------------------------

# add a source file (and corresponding header) to the given source list
function( add_ll_source PROJECT_ SOURCE_LIST_ FILE_ )

  # SOURCE_LIST is just the name of the list to append to, not a reference to the list itself
  # so we have to take the value of the name for the list command
  list( APPEND ${SOURCE_LIST_} ${FILE_} )
  #MESSAGE( STATUS "File list:" ${SRC_FILE_LIST} )

  # get the additional arguments (after the expected ones)
  set( ARGS ${ARGN} )

  list( FIND ARGS "HAS_PUBLIC_HEADER" hasPublicHeader )
  list( FIND ARGS "HAS_PRIVATE_HEADER" hasPrivateHeader )

  if( hasPublicHeader GREATER -1 )
    string( REGEX REPLACE ".([^.]+)$" ".h" header ${FILE_} )
    string( REGEX REPLACE "src/" "include/${PROJECT_}/" header ${header} )
    list( APPEND ${SOURCE_LIST_} ${header} )
  endif()

  if( hasPrivateHeader GREATER -1 )
    string( REGEX REPLACE ".([^.]+)$" ".h" header ${FILE_} )
    list( APPEND ${SOURCE_LIST_} ${header} )
  endif()

  # make the modifications available in the parent scope
  set( ${SOURCE_LIST_} ${${SOURCE_LIST_}} PARENT_SCOPE )

endfunction()


# -------------------------------------------------------------------------------------------------
# add_ll_module
# -------------------------------------------------------------------------------------------------
 
# add a ll module as dependency to a target
function( add_ll_module TARGET_ MODULE_ )

  add_dependencies( ${TARGET_} "ll_${MODULE_}" )

  if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    set( LIB_NAME "ll_${MODULE_}.lib" )          
  else()
    set( LIB_NAME "libll_${MODULE_}.a" )         
  endif()

  target_link_libraries( ${TARGET_} debug "${LL_LIBRARY_PATH_DEBUG}${LIB_NAME}" )
  target_link_libraries( ${TARGET_} optimized "${LL_LIBRARY_PATH_RELEASE}${LIB_NAME}" )
    
endfunction()

