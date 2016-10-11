#pragma once

#include "../support/environment.h"

#if LL_IS_WINDOWS()
#include <Windows.h>
#elif LL_IS_OSX()
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#endif

#include <map>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>


boost::filesystem::path get_executable_path()
{
#if LL_IS_WINDOWS()

  wchar_t buffer[MAX_PATH];
  ::GetModuleFileNameW( NULL, buffer, MAX_PATH );

#elif LL_IS_OSX()

  const size_t MAX_PATH = 1024;
  char buffer[MAX_PATH];
  uint32_t size = sizeof( buffer );
  _NSGetExecutablePath( buffer, &size );

#else

  const size_t MAX_PATH = 1024;
  char buffer[MAX_PATH];
  if( readlink( "/proc/self/exe", buffer, MAX_PATH ) < 0 )
    return boost::filesystem::path();

#endif

  boost::filesystem::path p( buffer );
  p.remove_filename();
  return p;
}