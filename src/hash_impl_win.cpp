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

#include "crypto/hash.h"

#include <Windows.h>
#include <bcrypt.h>

#include <map>
#include <algorithm>

#include "crypto/exception.h"
#include "internal_utils.h"


namespace ll
{
namespace crypto
{
  // ---------------------------------------------------------------------------------------------------------
  // hash_generator::concrete_hash_generator
  // ---------------------------------------------------------------------------------------------------------

  class hash_generator::concrete_hash_generator : public hash_generator
  {
  public:
    concrete_hash_generator( hash::type type_ );
    virtual ~concrete_hash_generator();

    void add_data( const std::uint8_t* pBuffer_, size_t sz_ ) override;
    hash retrieve_hash() override;

  private:
    hash::type m_type = hash::type::unknown;
    std::uint64_t m_inputSize = 0;

    BCRYPT_ALG_HANDLE m_hAlgorithm = NULL;
    BCRYPT_HASH_HANDLE m_hHash2 = NULL;
    std::vector< char > m_hashData;

    HCRYPTPROV m_hProvider = NULL;  // nullptr cannot be used here
    HCRYPTHASH m_hHash = NULL;
  };


  // ---------------------------------------------------------------------------------------------------------
  // hash_generator Implementation
  // ---------------------------------------------------------------------------------------------------------

  hash_generator::hash_generator( hash::type type_ )
    : m_pImpl( new hash_generator::concrete_hash_generator( type_ ) )
  {
  }


  // ---------------------------------------------------------------------------------------------------------
  // hash_generator::concrete_hash_generator Implementation
  // ---------------------------------------------------------------------------------------------------------

  hash_generator::concrete_hash_generator::concrete_hash_generator( hash::type type_ ) : m_type( type_ )
  {
    auto result = ::BCryptOpenAlgorithmProvider( &m_hAlgorithm, to_windows_hash_type( m_type ), NULL, 0 );
    if ( !BCRYPT_SUCCESS( result ) )
    {
      throw exception( error::internal, result );
    }

    ULONG hashObjectLength = 0;
    ULONG resultSize = 0;
    result = ::BCryptGetProperty( m_hAlgorithm, BCRYPT_OBJECT_LENGTH,
                                  reinterpret_cast< PUCHAR >( &hashObjectLength ), sizeof( hashObjectLength ),
                                  &resultSize, 0 );
    if ( !BCRYPT_SUCCESS( result ) )
    {
      throw exception( error::internal, result );
    }

    m_hashData.resize( hashObjectLength, 0 );
    result = ::BCryptCreateHash( m_hAlgorithm, &m_hHash2, reinterpret_cast< PUCHAR >( m_hashData.data() ),
                                 static_cast< ULONG >( m_hashData.size() ), NULL, 0, 0 );
    if ( !BCRYPT_SUCCESS( result ) )
    {
      throw exception( error::internal, result );
    }
  }


  // ---------------------------------------------------------------------------------------------------------

  hash_generator::concrete_hash_generator::~concrete_hash_generator()
  {
    if ( m_hHash2 != NULL )
      BCryptDestroyHash( m_hHash2 );
    if ( m_hAlgorithm )
      BCryptCloseAlgorithmProvider( m_hAlgorithm, 0 );
  }


  // ---------------------------------------------------------------------------------------------------------

  void hash_generator::concrete_hash_generator::add_data( const std::uint8_t* pBuffer_, size_t numBytes_ )
  {
    auto result = ::BCryptHashData( m_hHash2, reinterpret_cast< PUCHAR >( const_cast< uint8_t* >( pBuffer_ ) ),
                                    static_cast< ULONG >( numBytes_ ), 0 );
    if ( !BCRYPT_SUCCESS( result ) )
    {
      if( result == STATUS_INVALID_HANDLE )
        throw exception( error::invalid_request );
      else
        throw exception( error::internal, result );
    }

    m_inputSize += numBytes_;
  }


  // ---------------------------------------------------------------------------------------------------------

  hash hash_generator::concrete_hash_generator::retrieve_hash()
  {
    hash h;
    h.hashType = m_type;
    h.inputSize = m_inputSize;

    std::uint32_t hashLength = 0;
    ULONG resultSize = 0;
    auto result
      = ::BCryptGetProperty( m_hAlgorithm, BCRYPT_HASH_LENGTH, reinterpret_cast< PUCHAR >( &hashLength ),
                             sizeof( hashLength ), &resultSize, 0 );
    if ( !BCRYPT_SUCCESS( result ) )
    {
      throw exception( error::internal, result );
    }

    h.binary.resize( hashLength );
    result = BCryptFinishHash( m_hHash2, reinterpret_cast< PUCHAR >( h.binary.data() ), hashLength, 0 );
    if ( !BCRYPT_SUCCESS( result ) )
    {
      if( result == STATUS_INVALID_HANDLE )
        throw exception( error::invalid_request );
      else
        throw exception( error::internal, result );
    }

    return h;
  }


}  // namespace crypto
}  // namespace ll
