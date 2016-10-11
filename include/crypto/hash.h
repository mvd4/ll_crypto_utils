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

#include <vector>
#include <string>
#include <cstdint>
#include <memory>

#include "../support/environment.h"


namespace ll
{
namespace crypto
{
  struct hash
  {
    enum class type
    {
      unknown,
      md4,
      md5,
      sha1,
      sha256,
      sha384,
      sha512
    };

    struct config
    {
      size_t processingBlockSize = 100000;  //!< size of the internal block buffer for processing in bytes
    };


    type hashType = type::unknown;
    std::uint64_t inputSize = 0;  //!< the number of input bytes that generated this hash

    std::vector< std::uint8_t > binary;  //!< the binary representation of the hash
    std::string string;                  //!< the string representation of the hash
  };


  // ---------------------------------------------------------------------------------------------------------

  //! translate the hash type from a string to the enum
  hash::type to_hash_type( const std::string& type_ );

  //! translate the hash type from the enum to the (lowercase) string
  std::string to_string( hash::type type_ ) LL_NOEXCEPT;


  // ---------------------------------------------------------------------------------------------------------

  //! \todo test behaviour (e.g. multiple calls to retrieve, add_data after retrieve)
  class hash_generator
  {
  public:
    hash_generator( hash::type type_ );
    virtual ~hash_generator();

    hash_generator( hash_generator&& other_ );
    hash_generator& operator= ( hash_generator&& other_ );

    hash_generator( const hash_generator& other_ ) = delete;
    hash_generator& operator= ( const hash_generator& other_ ) = delete;

    virtual void add_data( const std::uint8_t* pBuffer_, size_t sz_ );
    virtual hash retrieve_hash();

  private:
    class concrete_hash_generator;

    hash_generator() {}

    std::unique_ptr< hash_generator > m_pImpl;
  };


  // ---------------------------------------------------------------------------------------------------------

  //! convenience: get the hash of a std::string
  hash get_hash( const std::string& v_,
                 hash::type type_ = hash::type::md5,
                 const hash::config& cfg_ = hash::config() );

  //! convenience: get the hash of a binary buffer
  hash get_hash( const void* pBuffer,
                 size_t szBufferInBytes_,
                 hash::type type_ = hash::type::md5,
                 const hash::config& cfg_ = hash::config() );

  //! convenience: get the hash of data from a stream (from the current position to end)
  hash get_hash( std::istream& stream_,
                 hash::type type_ = hash::type::md5,
                 const hash::config& cfg_ = hash::config() );

  

}  // namespace crypto
}  // namespace ll
