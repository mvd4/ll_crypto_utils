import sys
import os

from optparse import OptionParser


# ----------------------------------------------------------------------------------------------------------------------
# update the config.yaml file
def set_configuration_directory( lines_, dependency_, dir_identifier_ ):

  key = dir_identifier_ + "_dir"
  if not key in dependency_:
    return

  dir_value = dependency_[ key ]
  if not dir_value:
    return
  
  if not os.path.isabs( dir_value ):
    dir_value = os.getcwd() + "/" + dependency_[ "target_dir" ] + "/" + dir_value

  dir_value = dir_value.replace( "\\", "/" )  # unify to forward slashes

  dir_suffix = ""
  if dir_identifier_.endswith( "32" ):
    dir_identifier_ = dir_identifier_[:-2]    
  elif dir_identifier_.endswith( "64" ):
    dir_identifier_ = dir_identifier_[:-2]
    dir_suffix = " x64"

  dir_identifier = dependency_[ "name" ] + " " + dir_identifier_ + " dir" + dir_suffix
  
  for i, l in enumerate(lines_):
    if l.startswith( dir_identifier ):
      lines_[i] = l[:l.find(":") + 1 ] + " " + dir_value
 


# ----------------------------------------------------------------------------------------------------------------------
# update the config.yaml file
def update_configuration( dependencies_ ):
  
  print( "Updating configuration" )
  
  try:
    with open( "cfg/config.yaml", 'r') as f:
      lines = f.read().splitlines()
  except OSError as e:
     error( 'Could not read configuration file.' )

  for d in dependencies_:
    set_configuration_directory( lines, d, "include" )
    set_configuration_directory( lines, d, "library32" )
    set_configuration_directory( lines, d, "library64" )

  try:
    with open( "cfg/config.yaml", 'w') as f:
      for l in lines:
        f.write( l + "\n" )
  except OSError as e:
     error( 'Could not read configuration file.' )



# ----------------------------------------------------------------------------------------------------------------------
# 'main' function
def main( argv_ ):
  
  global cmdline_options

  usage = "%prog [options]"
  parser = OptionParser( usage=usage )
  
  parser.add_option( "-n", "--name", help="name of the dependency", type="string", action="store", dest="name" )
  parser.add_option( "-i", "--include", help="include path for this dependency", type="string", action="store", dest="include" )
  parser.add_option( "--lib32", help="library path for this dependency", type="string", action="store", dest="lib32" )
  parser.add_option( "--lib64", help="library path for this dependency", type="string", action="store", dest="lib64" )
       

 # try:                                
  (options, args) = parser.parse_args()
  #except getopt.GetoptError:           
   # usage()                          
    #sys.exit(2) 
  cmdline_options = vars( options )
  

  dependency = {}
  dependency[ "name" ] = cmdline_options[ "name" ]
  dependency[ "include_dir" ] = cmdline_options[ "include" ]
  dependency[ "library32_dir" ] = cmdline_options[ "lib32" ]
  dependency[ "library64_dir" ] = cmdline_options[ "lib64" ]

  dependencies = [ dependency ]
  update_configuration( dependencies )


# ------------------------------------------------------------------------------------------------------------
# entry point
if __name__ == "__main__":
  main( sys.argv )  # throw away first cmdline arg