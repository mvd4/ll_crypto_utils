cmake_minimum_required(VERSION 2.8)


# OS specific values
if( WIN32 )
  #set( LL_DEBUG_POSTFIX "d" )
  #set( LL_ARCHITECTURE_POSTFIX ".${LL_BUILD_ARCHITECTURE}" )
  #set( LL_VERSION_STRING ".${LL_LIBRARY_VERSION_MAJOR}.${LL_LIBRARY_VERSION_MINOR}" ) 
  set( LL_OUTPUT_SUBDIR "/${LL_BUILD_ARCHITECTURE}" )
elseif( APPLE )

else() # Linux etc

endif()

set( CMAKE_DEBUG_POSTFIX ${LL_DEBUG_POSTFIX} )


# Compiler specific settings
if( CMAKE_GENERATOR MATCHES "Unix Makefiles" )

  set( EXECUTABLE_OUTPUT_PATH ${LL_PROJECT_ROOT}/bin/${CMAKE_BUILD_TYPE} )
  set( LIBRARY_OUTPUT_PATH ${LL_PROJECT_ROOT}/lib/${CMAKE_BUILD_TYPE} )

  set( LL_LIBRARY_PATH_DEBUG "${LIBRARY_OUTPUT_PATH}/" )
  set( LL_LIBRARY_PATH_RELEASE "${LIBRARY_OUTPUT_PATH}/" )

else()

  set( EXECUTABLE_OUTPUT_PATH ${LL_PROJECT_ROOT}/bin${LL_OUTPUT_SUBDIR} )
  set( LIBRARY_OUTPUT_PATH ${LL_PROJECT_ROOT}/lib${LL_OUTPUT_SUBDIR} )

  set( LL_LIBRARY_PATH_DEBUG "${LIBRARY_OUTPUT_PATH}/Debug/" )
  set( LL_LIBRARY_PATH_RELEASE "${LIBRARY_OUTPUT_PATH}/Release/" )

endif()



# compiler specific
if( MSVC )
  
  # For MSVC, CMake sets certain flags to defaults we want to override.
  # This replacement code is taken from sample in the CMake Wiki at
  # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
  foreach (flag_var
           CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
           CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    
    # static runtime instead of default
    string(REPLACE "/MD" "-MT" ${flag_var} "${${flag_var}}")

    # warning level /W4 instead of default /W3
    string(REPLACE "/W3" "-W4" ${flag_var} "${${flag_var}}")
        
  endforeach()

  # enable catching SEH exceptions
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa")
  
  # enable some off-by-default compiler warnings 
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w44191 /w44242 /w44254 /w44255 /w44263 /w44264 /w44265 /w44266 /w44287 /w44289 /w44296 /w44302 /w44355 /w44431 /w44826 /w44928 /w44987")
  
  # disable "assignment operator could not be generated" warning
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4512" )
  
  
elseif( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")

  if( CMAKE_BUILD_TYPE MATCHES "Debug" )
    add_definitions( -DDEBUG )
  endif()


elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

endif()



