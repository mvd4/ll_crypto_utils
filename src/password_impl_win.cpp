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

#include <Windows.h>
#include <bcrypt.h>

#include <map>

#include "crypto/exception.h"
#include "internal_utils.h"


namespace ll
{
namespace crypto
{

  //! \todo string duplication with hash impl win
  namespace
  {
  }


  // ---------------------------------------------------------------------------------------------------------

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

    BCRYPT_ALG_HANDLE hAlgorithm = NULL;

    auto result = ::BCryptOpenAlgorithmProvider(
      &hAlgorithm, to_windows_hash_type( type_ ), NULL, BCRYPT_ALG_HANDLE_HMAC_FLAG );
    if ( !BCRYPT_SUCCESS( result ) )
    {
      throw exception( error::internal, result );
    }

    pbk pbk;
    pbk.binary.resize( cfg_.outputLength / 2 );  // we set the string length, not the binary length

    result = ::BCryptDeriveKeyPBKDF2(
      hAlgorithm, reinterpret_cast< PUCHAR >( const_cast< char* >( password_.data() ) ),
      static_cast< ULONG >( password_.length() ),
      reinterpret_cast< PUCHAR >( const_cast< char* >( salt_.data() ) ),
      static_cast< ULONG >( salt_.length() ), cfg_.numIterations,
      reinterpret_cast< PUCHAR >( pbk.binary.data() ), static_cast< ULONG >( pbk.binary.size() ), 0 );
    if ( !BCRYPT_SUCCESS( result ) )
    {
      throw exception( error::internal, result );
    }

    pbk.string = string_from_binary( pbk.binary );
    return pbk;
  }

}  // namespace crypto
}  // namespace ll
