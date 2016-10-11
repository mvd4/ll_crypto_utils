/*************************************************************************************************************

 Limelight Framework - Crypto Utils


 Copyright 2016 mvd

 Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software distributed under the License is
 distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and limitations under the License.

*************************************************************************************************************/

#pragma once


// -----------------------------------------------------------------------------------------------------------
// build platform
// -----------------------------------------------------------------------------------------------------------

// LL_ARCHITECTURE
#define LL_X64 1
#define LL_X32 2

#if defined _M_X64 || defined __x86_64
#define LL_ARCHITECTURE X64
#elif defined( __i386__ ) || defined _M_IX86
#define LL_ARCHITECTURE X32
#else
#error "Architecture not supported!"
#endif


// LL_PLATFORM
#define LL_WIN64 1
#define LL_WIN32 2
#define LL_OSX 3
#define LL_LINUX 4

#if defined WIN64 || ( defined _WINDOWS && ( LL_ARCHITECTURE == LL_X64 ) )
#define LL_PLATFORM LL_WIN64
#elif defined WIN32 || ( defined _WINDOWS && ( LL_ARCHITECTURE == LL_X32 ) )
#define LL_PLATFORM LL_WIN32
#elif defined __MACH__
#define LL_PLATFORM LL_OSX
#elif defined __linux__
#define LL_PLATFORM LL_LINUX
#else
#error "Platform not supported!"
#endif

#define LL_IS_WINDOWS() ( ( LL_PLATFORM == LL_WIN64 ) || ( LL_PLATFORM == LL_WIN32 ) )

#define LL_IS_OSX() ( LL_PLATFORM == LL_OSX )

#define LL_IS_LINUX() ( LL_PLATFORM == LL_LINUX )

#define LL_IS_POSIX() ( LL_IS_MAC || LL_IS_LINUX )


// LL_COMPILER
#define LL_MSVC 1
#define LL_CLANG 2
#define LL_GCC 3

#if defined _MSC_VER && _MSC_VER >= 1700
#define LL_COMPILER LL_MSVC
#define LL_COMPILER_VERSION _MSC_VER
#elif defined __GNUC__
#define LL_COMPILER LL_GCC
#define LL_COMPILER_VERSION ( __GNUC__ * 10 + __GNUC_MINOR__ )
#else
#error "Compiler not supported!"
#endif


// LL_CONFIG
#define LL_DEBUG 1
#define LL_RELEASE 2

#if defined DEBUG || defined _DEBUG
#define LL_CONFIG LL_DEBUG
#else
#define LL_CONFIG LL_RELEASE
#endif


// -----------------------------------------------------------------------------------------------------------
// compiler support
// -----------------------------------------------------------------------------------------------------------


#if LL_COMPILER == LL_MSVC 
#if LL_COMPILER_VERSION <= 1800
#define LL_HAS_MAKE_UNIQUUE() 1
#define LL_HAS_CONSTEXPR() 0
#define LL_HAS_NOEXCEPT() 0
#else
#define LL_HAS_MAKE_UNIQUUE() 1
#define LL_HAS_CONSTEXPR() 1
#define LL_HAS_NOEXCEPT() 1
#endif
#else
#define LL_HAS_MAKE_UNIQUUE() 0
#define LL_HAS_CONSTEXPR() 1
#define LL_HAS_NOEXCEPT() 1
#endif

#if LL_HAS_CONSTEXPR()
#define LL_CONSTEXPR constexpr
#else
#define LL_CONSTEXPR const
#endif

#if LL_HAS_NOEXCEPT()
#define LL_NOEXCEPT noexcept
#else
#define LL_NOEXCEPT
#endif
