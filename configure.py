import sys
import os
import getopt
import yaml
import platform
import shutil
import subprocess

from support.python import library_version


# ----------------------------------------------------------------------------------------------------------------------
# default arguments

architecture = 'x64'
configuration = 'Release'
configFile = 'cfg/config.yaml'

  
# ----------------------------------------------------------------------------------------------------------------------
# default configuration

libraryVersionMajor = '0'
libraryVersionMinor = '0'

buildDir = './build'
libraryDir = 'lib'
binaryDir = 'bin'

catchIncludeDir = ''

boostIncludeDir = ''
boostLibDir = ''


cmakeGenerator = ''


# ----------------------------------------------------------------------------------------------------------------------
# get the optional architecture suffix for VS builds
def get_cmake_generator_architecture():
  if ( platform.system() == 'Windows' ) and ( architecture == 'x64' ):
    return ' Win64'
  return ''

    
# ----------------------------------------------------------------------------------------------------------------------
# get the default cmake generator for this platform
def get_default_cmake_generator():
  if platform.system() == 'Windows':
    return 'Visual Studio 12 2013'
  elif platform.system() == 'Darwin':
    return 'Xcode'
  elif platform.system() == 'Linux':
    return 'Unix Makefiles'
  else:
    error( 'Unsupported platform!' )

    
# ----------------------------------------------------------------------------------------------------------------------
# call the cmake command
def run_cmake():
  global configuration
  global architecture
  global libraryVersionMajor
  global libraryVersionMinor

  print( 'Running CMake...' )
  
  args = ['cmake', '-G', cmakeGenerator ]
  args.append( '-DCMAKE_BUILD_TYPE=' + configuration ) 
  args.append( '-DLL_BUILD_ARCHITECTURE=' + architecture )
  args.append( '-DLL_LIBRARY_VERSION_MAJOR=' + libraryVersionMajor )
  args.append( '-DLL_LIBRARY_VERSION_MINOR=' + libraryVersionMinor )
  args.append( '..' )

  os.chdir( buildDir )
  subprocess.call( args )
  os.chdir( '..' )

  
# ----------------------------------------------------------------------------------------------------------------------
# generate the cmake configuration from the template
def configure_cmake():
  print( 'Creating CMake configuration...' )
  
  try:
    with open( './support/cmake/config.cmake.template', 'r') as f:
        template = f.readlines()
  except OSError as e:
    error( 'Could not read cmake configuration template.' )
  
  try:
    with open( './cfg/config.cmake', 'w') as out:
      for line in template:
        line = line.replace( '<the path to your catch include directory>', catchIncludeDir )
        line = line.replace( '<the path to your boost include directory>', boostIncludeDir )
        line = line.replace( '<the path to your boost library directory>', boostLibDir )
        
        out.write( line )
  except OSError as e:
    error( 'Could not create cmake configuration.' )

  
  
# ----------------------------------------------------------------------------------------------------------------------
# create the build folder if it does not exist, otherwise clean the CMake cache
def prepare_build_folder():
  if not os.path.exists( buildDir ):
    print( 'Creating build directory...' )
    os.makedirs( buildDir )
  else:
    print( 'Cleaning CMake caches in build directory...' )
    if os.path.exists( buildDir + '/CMakeCache.txt' ):
      os.remove( buildDir + '/CMakeCache.txt' )
    if os.path.exists( buildDir + '/CMakeFiles' ):
      shutil.rmtree( buildDir + '/CMakeFiles' )


# ----------------------------------------------------------------------------------------------------------------------
# read the library version from the c++ header
def read_library_version():
  global libraryVersionMajor
  global libraryVersionMinor
  
  version = library_version.read_library_version()
  libraryVersionMajor = version[0]
  libraryVersionMinor = version[1]


# ----------------------------------------------------------------------------------------------------------------------
# load the configuration from the given yaml file
def load_configuration( cfgFile_ ):
  
  global buildDir
  global libraryDir
  global binaryDir

  global catchIncludeDir
  
  global boostIncludeDir
  global boostLibDir
  boostLibDir64 = ''
     
  global cmakeGenerator

  
  try:
    with open( cfgFile_, 'rb') as f:
      config = yaml.load( f )
  except OSError as e:
     error( 'Could not read configuration file.' )
    
    
  if config[ 'build dir' ]:
    buildDir = config[ 'build dir' ]
  if config[ 'library output dir' ]:
    libraryDir = config[ 'library output dir' ]
  if config[ 'binary output dir' ]:
    binaryDir = config[ 'binary output dir' ]
  
  if config[ 'catch include dir' ]:
    catchIncludeDir = config[ 'catch include dir' ]
  
  if config[ 'boost include dir' ]:
    boostIncludeDir = config[ 'boost include dir' ]
  if config[ 'boost library dir' ]:
    boostLibDir = config[ 'boost library dir' ]
  if config[ 'boost library dir x64' ]:
    boostLibDir64 = config[ 'boost library dir x64' ]
    
  
  # assign x64 dirs to standard lib dirs    
  if ( platform.system() == 'Windows' ) and ( architecture == 'x64' ):
    boostLibDir = boostLibDir64
   
   
  if config[ 'cmake generator' ]:
    cmakeGenerator = config[ 'cmake generator' ]    
  else:
    cmakeGenerator = get_default_cmake_generator()

  cmakeGenerator += get_cmake_generator_architecture()
  

# ----------------------------------------------------------------------------------------------------------------------
# output error msg and exit
def error( msg_ ):
  print( 'ERROR: ' + msg_ )  
  sys.exit( 2 )
  
  
# ----------------------------------------------------------------------------------------------------------------------
# usage hints
def usage():
  print( 'LibLimelight build script' )
  print( 'Usage:' )
  print( '  -a, --arch: specify build architecture [x32, x64 (default)]' )
  print( '  -c, --config: specify build config [Debug, Release (default)]' )
  print( '  -f, --cfgfile: use this configuration yaml file' )
  print( '  -h, --help: This help' )
  
  
# ----------------------------------------------------------------------------------------------------------------------
# 'main' function
def main( argv_ ):
  global configuration
  global architecture
  global configFile
  
  try:                                
    opts, args = getopt.getopt( argv_, 'ha:c:f:', [ 'help', 'arch=', 'config=', 'cfgfile=' ] ) 
  except getopt.GetoptError:           
   usage()                          
   sys.exit(2) 

  for opt, arg in opts:                
    if opt in ( '-h', '--help' ):      
      usage()                     
      sys.exit()                  
    elif opt in ( '-a', '--arch' ): 
      architecture = arg   
    elif opt in ( '-c', '--config' ): 
      configuration = arg
    elif opt in ( '-f', '--cfgfile' ): 
      configFile = arg
  
  load_configuration( configFile )
  read_library_version()
  prepare_build_folder()
  configure_cmake()
  run_cmake()
  

# ------------------------------------------------------------------------------------------------------------
# entry point
if __name__ == "__main__":
  main( sys.argv[1:] )  # throw away first cmdline arg