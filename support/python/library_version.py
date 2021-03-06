import sys
import os
import subprocess

from optparse import OptionParser

# ----------------------------------------------------------------------------------------------------------------------
# read the library version from the c++ header
def read_library_version():
  
  try:
    with open( "./include/crypto/version.h", 'r') as f:
      for line in f:
        if line.find( "libraryVersionMajor" ) != -1:
          libraryVersionMajor = line[ line.find( "=" ) + 1 : line.find(";") ].strip()
        if line.find( "libraryVersionMinor" ) != -1:
          libraryVersionMinor = line[ line.find( "=" ) + 1 : line.find(";") ].strip()
        if line.find( "libraryVersionMicro" ) != -1:
          libraryVersionMicro = line[ line.find( "=" ) + 1 : line.find(";") ].strip()
          
  except OSError as e:
     error( 'Could not read library version.' )

  
  return [ libraryVersionMajor, libraryVersionMinor, libraryVersionMicro ]


# ----------------------------------------------------------------------------------------------------------------------
# 'main' function
def main( argv_ ):

  usage = "%prog [options]"
  parser = OptionParser( usage=usage )
  
  parser.add_option( "--appveyor", help="update appveyor environment variable", action="store_true", dest="appveyor", default=False )
       

 # try:                                
  (options, args) = parser.parse_args()
  #except getopt.GetoptError:           
   # usage()                          
    #sys.exit(2) 
  cmdline_options = vars( options )

  version = read_library_version()
  versionstring = '%s' % '.'.join(map(str, version) )
  
  if "appveyor" in cmdline_options and cmdline_options[ "appveyor" ]:
    versionstring = versionstring + "." + os.environ['APPVEYOR_BUILD_NUMBER']
    
    args = ['appveyor', 'UpdateBuild', '-Version', versionstring ]
    subprocess.call( args )  

    #args = ['appveyor', 'SetVariable', '-Name', 'APPVEYOR_BUILD_VERSION', '-Value', versionstring ]
    #subprocess.call( args )   

  print ( versionstring )


# ------------------------------------------------------------------------------------------------------------
# entry point
if __name__ == "__main__":
  main( sys.argv[1:] )  # throw away first cmdline arg