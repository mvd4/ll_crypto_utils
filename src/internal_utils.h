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

#include "../support/environment.h"

#if LL_IS_WINDOWS()
#include <Windows.h>
#endif

#include <vector>
#include <string>
#include <cstdint>
#include <functional>

#include "crypto/exception.h"


namespace ll
{
namespace crypto
{
  inline static std::string string_from_binary( const std::vector< std::uint8_t >& binary_ ) LL_NOEXCEPT
  {
    static const char kEncodingTable[17] = "0123456789abcdef";

    // encode string
    std::string result;
    for ( auto b : binary_ )
    {
      result += kEncodingTable[( b >> 4 ) & 0xf];
      result += kEncodingTable[b & 0xf];
    }

    return result;
  }


// ---------------------------------------------------------------------------------------------------------

#if LL_IS_WINDOWS()

  inline static LPCWSTR to_windows_hash_type( ll::crypto::hash::type type_ )
  {
    switch ( type_ )
    {
      case ll::crypto::hash::type::md4:
        return BCRYPT_MD4_ALGORITHM;
      case ll::crypto::hash::type::md5:
        return BCRYPT_MD5_ALGORITHM;
      case ll::crypto::hash::type::sha1:
        return BCRYPT_SHA1_ALGORITHM;
      case ll::crypto::hash::type::sha256:
        return BCRYPT_SHA256_ALGORITHM;
      case ll::crypto::hash::type::sha384:
        return BCRYPT_SHA384_ALGORITHM;
      case ll::crypto::hash::type::sha512:
        return BCRYPT_SHA512_ALGORITHM;
      default:
        throw exception( error::invalid_parameter, "Unsupported hash type" );
    }
  }

#endif

}  // namespace crypto
}  // namespace ll
