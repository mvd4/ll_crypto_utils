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

#include <map>
#include <algorithm>
#include <iostream>

#include "crypto/exception.h"
#include "internal_utils.h"

#include "../support/debug_helpers.h"


#define LL_HASHTYPE_TABLE()                          \
  LL_TRANSLATE_ENUM( hash::type::md4, "md4" )        \
  LL_TRANSLATE_ENUM( hash::type::md5, "md5" )        \
  LL_TRANSLATE_ENUM( hash::type::sha1, "sha-1" )     \
  LL_TRANSLATE_ENUM( hash::type::sha256, "sha-256" ) \
  LL_TRANSLATE_ENUM( hash::type::sha384, "sha-384" ) \
  LL_TRANSLATE_ENUM( hash::type::sha512, "sha-512" )


namespace ll
{
namespace crypto
{

  // -----------------------------------------------------------------------------------------------------------
  // utilities
  // -----------------------------------------------------------------------------------------------------------

  namespace
  {
    hash invoke_hash_generator( const uint8_t* pBuffer,
                          size_t szBufferInBytes_,
                          hash::type type_,
                          const hash::config& cfg_ )
    {
      hash_generator calculator( type_ );

      while ( szBufferInBytes_ >= cfg_.processingBlockSize )
      {
        calculator.add_data( pBuffer, cfg_.processingBlockSize );
        pBuffer += cfg_.processingBlockSize;
        szBufferInBytes_ -= cfg_.processingBlockSize;
      }
      calculator.add_data( pBuffer, szBufferInBytes_ );

      return calculator.retrieve_hash();
    }


    // ---------------------------------------------------------------------------------------------------------

    hash invoke_hash_generator( std::istream& stream_, hash::type type_, const hash::config& cfg_ )
    {
      hash_generator calculator( type_ );

      std::vector< std::uint8_t > data( cfg_.processingBlockSize );

	  std::uint64_t bytesRead = 0;
      std::uint64_t bytesTotal = 0;
      while ( stream_.good() )
      {
        stream_.read( reinterpret_cast< char* >( data.data() ), cfg_.processingBlockSize );
        bytesRead = stream_.gcount();
        calculator.add_data( data.data(), static_cast< size_t >( bytesRead ) );
        bytesTotal += bytesRead;
      }

      return calculator.retrieve_hash();
    }
  }


  // ---------------------------------------------------------------------------------------------------------
  // hash_generator Implementation
  // ---------------------------------------------------------------------------------------------------------

  hash_generator::~hash_generator() = default;

  hash_generator::hash_generator( hash_generator&& other_ )
  {
    m_pImpl.reset( other_.m_pImpl.release() );
  }

  hash_generator& hash_generator::operator=( hash_generator&& other_ )
  {
    m_pImpl.reset( other_.m_pImpl.release() );
    return *this;
  }

  void hash_generator::add_data( const std::uint8_t* pBuffer_, size_t sz_ )
  {
    LL_PRECONDITION( ( pBuffer_ != nullptr ) || ( sz_ == 0 ) );
    m_pImpl->add_data( pBuffer_, sz_ );
  }

  hash hash_generator::retrieve_hash()
  {
    auto h = m_pImpl->retrieve_hash();
    h.string = string_from_binary( h.binary );
    return h;
  }


  // -----------------------------------------------------------------------------------------------------------
  // hash functions implementation
  // -----------------------------------------------------------------------------------------------------------


  hash::type to_hash_type( const std::string& type_ )
  {
    static std::map< std::string, hash::type > s_hashTypes = {
#define LL_TRANSLATE_ENUM( enum_, string_ ) \
  {                                         \
    string_, enum_                          \
  }                                         \
  ,
      LL_HASHTYPE_TABLE()
#undef LL_TRANSLATE_ENUM
    };

    auto type = type_;
    std::transform( type.begin(), type.end(), type.begin(), ::tolower );
    auto it = s_hashTypes.find( type );
    return ( it == s_hashTypes.end() ) ? hash::type::unknown : it->second;
  }


  // ---------------------------------------------------------------------------------------------------------

  std::string to_string( hash::type type_ ) LL_NOEXCEPT
  {
    switch ( type_ )
    {
#define LL_TRANSLATE_ENUM( enum_, string_ ) \
  case enum_:                               \
    return string_;
      LL_HASHTYPE_TABLE()
#undef LL_TRANSLATE_ENUM
      default:
        return "unknown";
    }
  }

#undef M_HASHTYPE_TABLE


  hash get_hash( const std::string& str_, hash::type type_, const hash::config& cfg_ )
  {
    return get_hash( str_.data(), str_.size(), type_, cfg_ );
  }

  
  // ---------------------------------------------------------------------------------------------------------

  hash get_hash( const void* pBuffer_, size_t szBufferInBytes_, hash::type type_, const hash::config& cfg_ )
  {
    if ( !pBuffer_ && ( szBufferInBytes_ > 0 ) )
      throw exception( error::invalid_parameter, "invalid buffer" );

    if ( type_ == hash::type::unknown )
      throw exception( error::invalid_parameter, "invalid hash type" );


    return invoke_hash_generator( static_cast< const uint8_t* >( pBuffer_ ), szBufferInBytes_, type_, cfg_ );    
  }


  // ---------------------------------------------------------------------------------------------------------

  hash get_hash( std::istream& stream_, hash::type type_, const hash::config& cfg_ )
  {
    if ( !stream_ )
      throw exception( error::invalid_parameter, "invalid stream" );

    if ( type_ == hash::type::unknown )
      throw exception( error::invalid_parameter, "invalid hash type" );


    return invoke_hash_generator( stream_, type_, cfg_ );    
  }

}  // namespace crypto
}  // namespace ll
