Limelight Crypto-Utils
======================

Welcome to the Limelight Crypto-Utils!

This library provides a cross-platform toolkit for common cryptographic usecases with a simple interface.

Crypto-Utils doesn't implement any of the security-relevant algorithms itself, but rather uses the native implementation on the respective platform, i.e.

* the BCrypt API on Windows
* the CommonCrypto framework on OSX
* OpenSSL on Linux

The library is currently in a very early stage, so the featureset is small. It will be extended in the near future.


Features
--------
* hash generation for strings, files and arbitrary data blocks
    * supported algorithms: MD4, MD5, SHA1, SHA-256, SHA-384, SHA-512
* utility functions for password-hashing
    * pbkdf2
* modern C++11 code
    
    
License
-------

crypto_utils is licensed under the Apache License, Version 2.0 ( http://www.apache.org/licenses/LICENSE-2.0 )


Supported platforms
-------------------
* Windows >= 7
* OSX >= 10.8
* any Linux with OpenSSL


Dependencies
------------
Crypto-Utils is currently dependent on the following 3rd party libraries:

* boost (filesystem, system) >= 1.56
* openSSL (only on Linux) >= 1.0.1

Additionally you might require the following additional software

* CMake >= 2.8 (for creating the build-projects)
* Catch >= 1.56 (for building the tests) 
* python3

*Older versions of these libraries might work but were not tested against*


Building Crypto-Utils
---------------------
Crypto-Utils are known to work with the following C++11 compilers

* VS >= 2013
* LLVM/Clang
* GCC >= 4.9

To build it, you have two options:

### Option 1: Using the convenience build scripts
If you have a working python3 installation at hand, this is probably the easiest way to get started.

#### 1. Preparation
* clone this project into a directory of your choice
* create a python 3 virtual environment in the crypto_utils root folder and activate it

#### 2. Bootstrapping
Bootstrapping is the process of setting up your environment so that the crypto_utils can be built with all their dependencies.
Instead of doing this manually, you can utilize the bootstrap script that comes with the library. To do so:
* run the bootstrap.py script from within the virtual environment (which you created and activated in step 1). It will
  * install the PyYaml package via pip (needed for configuring the build later)
  * checkout the dependencies (Catch,boost) to a folder parallel to the crypto_utils root folder
  * automatically configure the include and library paths for the dependencies

In some cases you will not want the bootstrap script to checkout all the dependencies. 
E.g. you might already have boost installed on your system, potentially in a different version. 
If this is the case, simply add an argument *--without [dependency name]* when running the script (it's possible to add this flag multiple times).

If you do not want to checkout any of the dependencies, simply pass the flag *--no-checkout*.

In case you want bootstrap.py to checkout the dependencies, but want to use a different version or have them installed in another location,
simply edit the file *cfg/dependencies.xml* according to your requirements before running the script.

#### 3. Configuring the build

If you used the bootstrap.py script to checkout all dependencies, you're ready to run the configuration script.
If not, you'll need to tell the build scripts where to find the dependencies. In order to do that:
* open the file *cfg/config.yaml* and set all required paths correctly (the 64bit paths are only needed on windows)

Now you're all set up to configure the build. Configuring the build means to select a compiler, a configuration (Debug/Release), an architecture (on windows/linux) etc...
In order to do this 
* run the configure.py script from within the virtual environment (which you created and activated in step 1).
  * use the *--help* switch to get a list of options

What this does is that it creates / cleans up the build folder  and runs CMake to create the selected buildfiles.  
You can now use the CMake-generated project files to build the crypto_utils. 

If you want to change the build directory or output folders for libraries and binaries, edit the file *cfg/config.yaml* before running configure.py.


### Option 2: Using CMake directly
If you don't want to use the convenience-scripts or need more control over the build, you can of course also use CMake directly. In that case

#### 1. Preparation
* clone this project into a directory of your choice
* copy the file *./support/cmake/build-settings.cmake.template* to */cfg* and remove the *.template* extension.
* edit *./cfg/build-settings.cmake by specifying the paths to the locations of the required dependencies on your system

#### 2. Configuring the build
* run CMake with the desired options from the build-folder of your choice
