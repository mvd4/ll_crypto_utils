import sys
import os
import platform
import shutil
import subprocess

from optparse import OptionParser

from support.python import dependencies, configuration


# ----------------------------------------------------------------------------------------------------------------------
# globals
cmdline_options = []


# ----------------------------------------------------------------------------------------------------------------------
# install all required python extensions
def setup_python_environment():
  args = ['pip', 'install', 'PyYaml' ]
  
  subprocess.call( args )
  

# ----------------------------------------------------------------------------------------------------------------------
# output error msg and exit
def error( msg_ ):
  print( 'ERROR: ' + msg_ )  
  sys.exit( 2 )
 
  
  
# ----------------------------------------------------------------------------------------------------------------------
# 'main' function
def main( argv_ ):
  
  global cmdline_options

  print( "LibLimelight bootstrap script" )

  usage = "%prog [options]"
  parser = OptionParser( usage=usage )
  
  parser.add_option( "-n", "--no-checkout", help="do not checkout any of the 3rd party dependencies", action="store_true", dest="no_checkout" )
  parser.add_option( "-w", "--with", help="explicitly set a 3rd party dependency to be checked out", action="append", type="string", dest="with" )
  parser.add_option( "--without", help="explicitly exclude a 3rd party dependency from being checked out", action="append", type="string", dest="without" )

 # try:                                
  (options, args) = parser.parse_args()
  #except getopt.GetoptError:           
   # usage()                          
    #sys.exit(2) 
  cmdline_options = vars( options )
  

  setup_python_environment()
  deps = dependencies.load_dependencies( cmdline_options )
  dependencies.checkout_dependencies( deps )
  configuration.update_configuration( deps )
  

# ------------------------------------------------------------------------------------------------------------
# entry point
if __name__ == "__main__":
  main( sys.argv )  # throw away first cmdline arg