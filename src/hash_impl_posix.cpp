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

#include <iostream>
#include <map>
#include <vector>

#include "../support/environment.h"

#if LL_IS_OSX()
#define COMMON_DIGEST_FOR_OPENSSL 1
#include <CommonCrypto/CommonDigest.h>
#else  // linux
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#endif

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
    virtual ~concrete_hash_generator() = default;

    void add_data( const std::uint8_t* pBuffer_, size_t sz_ ) override;
    hash retrieve_hash() override;

  private:
    // the concept declares the interface requirements on the templated models
    struct context_concept_t
    {
      virtual ~context_concept_t() {}
      virtual void update( const std::uint8_t* pBuffer_, size_t numBytes_ ) = 0;
      virtual void get_hash_binary( std::vector< std::uint8_t >& buffer_ ) = 0;
    };

    // templated model to hold the actual context
    template < typename context_type_t >
    struct context_model_t : context_concept_t
    {
      context_model_t( hash::type type_ = hash::type::unknown );

      void update( const std::uint8_t* pBuffer_, size_t numBytes_ ) override
      {
        if ( !m_fnFinal )
          throw exception( error::invalid_request );
      
        //! \todo check that numBytes fits in 32bit and throw exception otherwise
        auto result = m_fnUpdate( pBuffer_, static_cast< std::uint32_t >( numBytes_ ) );
        if( result != 1 )
        {
          throw crypto::exception( error::internal );
        }
      }

      void get_hash_binary( std::vector< std::uint8_t >& buffer_ ) override
      {
        if ( !m_fnFinal )
          throw exception( error::invalid_request );
      
        buffer_.resize( m_szHash );
        auto result = m_fnFinal( reinterpret_cast< unsigned char* >( buffer_.data() ) );
        if( result != 1 )
        {
          throw crypto::exception( error::internal );
        }
        m_fnFinal = final_func_t();  // reset the function to emulate behaviour on windows
      }

    private:
      using update_func_t = std::function< int( const void*, std::uint32_t ) >;
      using final_func_t = std::function< int(std::uint8_t*)>;

      context_type_t m_context;
      size_t m_szHash;
      update_func_t m_fnUpdate;
      final_func_t m_fnFinal;
    };

    hash::type m_type = hash::type::unknown;
    std::uint64_t m_inputSize = 0;

    std::unique_ptr< context_concept_t > m_pImpl;
  };


  // ---------------------------------------------------------------------------------------------------------

  template <>
  hash_generator::concrete_hash_generator::context_model_t< MD4_CTX >::context_model_t( hash::type )
    : m_context( MD4_CTX() ), m_szHash( MD4_DIGEST_LENGTH )
  {
    MD4_Init( &m_context );
    m_fnUpdate = std::bind( MD4_Update, &m_context, std::placeholders::_1, std::placeholders::_2 );
    m_fnFinal = std::bind( MD4_Final, std::placeholders::_1, &m_context );
  }

  template <>
  hash_generator::concrete_hash_generator::context_model_t< MD5_CTX >::context_model_t( hash::type )
    : m_context( MD5_CTX() ), m_szHash( MD5_DIGEST_LENGTH )
  {
    MD5_Init( &m_context );
    m_fnUpdate = std::bind( MD5_Update, &m_context, std::placeholders::_1, std::placeholders::_2 );
    m_fnFinal = std::bind( MD5_Final, std::placeholders::_1, &m_context );
  }

  template <>
  hash_generator::concrete_hash_generator::context_model_t< SHA_CTX >::context_model_t( hash::type )
    : m_context( SHA_CTX() ), m_szHash( SHA_DIGEST_LENGTH )
  {
    SHA1_Init( &m_context );
    m_fnUpdate = std::bind( SHA1_Update, &m_context, std::placeholders::_1, std::placeholders::_2 );
    m_fnFinal = std::bind( SHA1_Final, std::placeholders::_1, &m_context );
  }

  template <>
  hash_generator::concrete_hash_generator::context_model_t< SHA256_CTX >::context_model_t( hash::type )
    : m_context( SHA256_CTX() ), m_szHash( SHA256_DIGEST_LENGTH )
  {
    SHA256_Init( &m_context );
    m_fnUpdate = std::bind( SHA256_Update, &m_context, std::placeholders::_1, std::placeholders::_2 );
    m_fnFinal = std::bind( SHA256_Final, std::placeholders::_1, &m_context );
  }

  template <>
  hash_generator::concrete_hash_generator::context_model_t< SHA512_CTX >::context_model_t( hash::type type_ )
    : m_context( SHA512_CTX() )
    , m_szHash( ( type_ == hash::type::sha384 ) ? SHA384_DIGEST_LENGTH : SHA512_DIGEST_LENGTH )
  {
    if ( type_ == hash::type::sha384 )
    {
      SHA384_Init( &m_context );
      m_fnUpdate = std::bind( SHA384_Update, &m_context, std::placeholders::_1, std::placeholders::_2 );
      m_fnFinal = std::bind( SHA384_Final, std::placeholders::_1, &m_context );
    }
    else
    {
      SHA512_Init( &m_context );
      m_fnUpdate = std::bind( SHA512_Update, &m_context, std::placeholders::_1, std::placeholders::_2 );
      m_fnFinal = std::bind( SHA512_Final, std::placeholders::_1, &m_context );
    }
  }


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
    switch ( m_type )
    {
      case hash::type::md4:
        m_pImpl = std::unique_ptr< context_concept_t >( new context_model_t< MD4_CTX >() );
        break;
      case hash::type::md5:
        m_pImpl = std::unique_ptr< context_concept_t >( new context_model_t< MD5_CTX >() );
        break;
      case hash::type::sha1:
        m_pImpl = std::unique_ptr< context_concept_t >( new context_model_t< SHA_CTX >() );
        break;
      case hash::type::sha256:
        m_pImpl = std::unique_ptr< context_concept_t >( new context_model_t< SHA256_CTX >() );
        break;
      case hash::type::sha384:
        m_pImpl = std::unique_ptr< context_concept_t >( new context_model_t< SHA512_CTX >( m_type ) );
        break;
      case hash::type::sha512:
        m_pImpl = std::unique_ptr< context_concept_t >( new context_model_t< SHA512_CTX >() );
        break;
      default:
        throw exception( error::invalid_parameter, "Unsupported hash type" );
    }
  }


  // ---------------------------------------------------------------------------------------------------------

  void hash_generator::concrete_hash_generator::add_data( const std::uint8_t* pBuffer_, size_t numBytes_ )
  {
    m_pImpl->update( pBuffer_, numBytes_ );
    m_inputSize += numBytes_;
  }


  // ---------------------------------------------------------------------------------------------------------

  hash hash_generator::concrete_hash_generator::retrieve_hash()
  {
    hash h;
    h.hashType = m_type;
    h.inputSize = m_inputSize;
    m_pImpl->get_hash_binary( h.binary );
    return h;
  }


}  // namespace crypto
}  // namespace ll
