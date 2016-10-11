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

#include "crypto/password.h"

#include "../support/environment.h"

#if LL_IS_OSX()
#define COMMON_DIGEST_FOR_OPENSSL 1
#include <CommonCrypto/CommonDigest.h>
#include <CommonCrypto/CommonKeyDerivation.h>
#else  // linux
#include <openssl/evp.h>
#endif

#include "internal_utils.h"


namespace ll
{
namespace crypto
{
  namespace
  {
#if LL_IS_OSX()
    
    inline static int to_osx_hash_type( ll::crypto::hash::type type_ )
    {
      switch ( type_ )
      {
        case ll::crypto::hash::type::sha1:
          return kCCPRFHmacAlgSHA1;
        case ll::crypto::hash::type::sha256:
          return kCCPRFHmacAlgSHA256;
        case ll::crypto::hash::type::sha384:
          return kCCPRFHmacAlgSHA384;
        case ll::crypto::hash::type::sha512:
          return kCCPRFHmacAlgSHA512;
        default:
          throw exception( error::invalid_parameter, "Unsupported hash type" );
      }
    }

#else
    
    inline static const EVP_MD* to_openssl_hash_type( ll::crypto::hash::type type_ )
    {
      switch ( type_ )
      {
        case ll::crypto::hash::type::md4:
          return EVP_md4();
        case ll::crypto::hash::type::md5:
          return EVP_md5();
        case ll::crypto::hash::type::sha1:
          return EVP_sha1();
        case ll::crypto::hash::type::sha256:
          return EVP_sha256();
        case ll::crypto::hash::type::sha384:
          return EVP_sha384();
        case ll::crypto::hash::type::sha512:
          return EVP_sha512();
        default:
          throw exception( error::invalid_parameter, "Unsupported hash type" );
      }
    }

#endif
  }

  // -------------------------------------------------------------------------------------------------------

  pbk pbkdf2( const std::string& password_, const std::string& salt_, hash::type type_, pbk::config cfg_ )
  {
    if( 
      password_.empty() ||
      ( type_ == hash::type::unknown ) || 
      ( cfg_.numIterations == 0 ) || 
      ( cfg_.outputLength == 0 ) 
    )
    {
      throw crypto::exception( error::invalid_parameter );
    }

    pbk pbk;
    pbk.binary.resize( cfg_.outputLength / 2 );  // cfg sets the string length, which is 2* binary

#if LL_IS_OSX()    

    for ( unsigned i = 0; i < cfg_.numIterations + 1; ++i )
    {
      CCKeyDerivationPBKDF( kCCPBKDF2, password_.data(), password_.size(),
                            reinterpret_cast< const std::uint8_t* >( salt_.data() ), salt_.size(),
                            to_osx_hash_type( type_ ), i, pbk.binary.data(), pbk.binary.size() );
    }
    

#else

    if( PKCS5_PBKDF2_HMAC( password_.c_str(), password_.size(),
                           reinterpret_cast< const unsigned char* >( salt_.c_str() ), salt_.size(),
                           cfg_.numIterations,
                           to_openssl_hash_type( type_ ),
                           pbk.binary.size(), pbk.binary.data() ) != 1 )
    {
      throw crypto::exception( error::internal );
    }


#endif

    pbk.string = string_from_binary( pbk.binary );
    return pbk;
  }

}  // namespace crypto
}  // namespace ll
