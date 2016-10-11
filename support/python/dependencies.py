import sys
import os
import platform
import shutil
import subprocess

import xml.etree.ElementTree as et


# ----------------------------------------------------------------------------------------------------------------------
# globals
dependencies = []
  

# ----------------------------------------------------------------------------------------------------------------------
# 
def move_recursively( src_, tgt_ ):
  names = os.listdir( src_ )
  for n in names:
    source_item = src_ + "/" + n
    target_item = tgt_ + "/" + n

    # check if target folder already exists, if so integrate instead of replacing
    if not os.path.isdir( target_item ):
      shutil.move( source_item, target_item )
    else:
      move_recursively( source_item, target_item )
      

# ----------------------------------------------------------------------------------------------------------------------
# checkout all submodules for this dependency
def checkout_submodules( dependency_ ):

  if not "submodules" in dependency_:
    return

  cwd = os.getcwd()
  os.chdir( dependency_[ "target_dir" ] )

  args = ['git', 'submodule', 'init' ]
  for s in dependency_[ "submodules" ]:
    args.append( s )
    
  subprocess.call( args )

  args = ['git', 'submodule', 'update', '--recursive' ]
  subprocess.call( args )

  if dependency_[ "name" ].startswith( "boost" ):
    os.mkdir( "boost" )
    for s in dependency_[ "submodules" ]:
      
      include_dir = s + "/include/boost/"
      if not os.path.isdir( include_dir ):
        continue
      
      move_recursively( include_dir, "./boost" )
      
    if platform.system() == 'Windows':
      bootstrap_command = "bootstrap.bat"
      b2_command = "b2.exe"
    else:
      bootstrap_command = "./bootstrap.sh"
      b2_command = "./b2" 

    args = [ bootstrap_command ]
    subprocess.call( args )

    args = [ b2_command, "--with-system", "--with-filesystem" ]
    subprocess.call( args )
    
  os.chdir( cwd )


# ----------------------------------------------------------------------------------------------------------------------
# checkout all dependencies in the list
def checkout_dependencies( dependencies_ ):

  for d in dependencies_:
    print( "Checking out " + d[ "name" ] + " to " + d[ "target_dir" ] + " ..." )
    if not os.path.isdir( d[ "target_dir" ] ):
      os.mkdir( d[ "target_dir" ] )

      args = ['git', 'clone', d[ "url" ], d[ "target_dir" ], "--branch", d[ "tag" ] ]
      subprocess.call( args )

      checkout_submodules( d )

    else:
      print( "... already exists" )


# ----------------------------------------------------------------------------------------------------------------------
# read the list of 3rd party dependencies to checkout
def load_dependencies( options_ ):
  
  if options_[ "no_checkout" ]:
    return

  tree = et.parse( 'cfg/dependencies.xml' )
  root = tree.getroot()

  for d in root:
    dependency = {}
    dependency.update( d.attrib )
      
    for t in d:
      if len( list( t ) ) > 1:
        dependency[ t.tag ] = []
        for s in t:
          dependency[ t.tag ].append( s.text )
      else:
        dependency[ t.tag ] = t.text
      
  
    if options_[ "with" ] and ( dependency[ "name" ] not in options_[ "with" ] ):
      continue
    if options_[ "without" ] and ( dependency[ "name" ] in options_[ "without" ] ):
      continue
      
    dependencies.append( dependency )
    
  return dependencies


# ----------------------------------------------------------------------------------------------------------------------
# 'main' function
def main( argv_ ):
  
  dependencies.load_dependencies()
  dependencies.checkout_dependencies()


# ------------------------------------------------------------------------------------------------------------
# entry point
if __name__ == "__main__":
  main( sys.argv )  # throw away first cmdline arg