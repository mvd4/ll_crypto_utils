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

#include <catch.hpp>

#include <map>
#include <mutex>
#include <future>
#include <condition_variable>
#include <random>

#include <crypto/hash.h>
#include <crypto/exception.h>

#include "../helpers/test_helpers.h"


namespace ll
{
namespace crypto
{
  namespace test
  {
    class random_block_size
    {
    public:
      random_block_size()
        : m_rd()
        , m_mt19937( m_rd() )
      {
      }

      size_t get()
      {
        std::uniform_int_distribution< size_t > dist( 100, 100000 );
        return dist( m_mt19937 );
      }


    private:

      std::random_device m_rd;
      std::mt19937 m_mt19937;
    };


    // -------------------------------------------------------------------------------------------------------

    TEST_CASE( "hash type conversion" )
    {
      SECTION( "to_hash_type is case insensitive" )
      {
        CHECK( hash::type::md4 == to_hash_type( "md4" ) );
        CHECK( hash::type::md5 == to_hash_type( "md5" ) );
        CHECK( hash::type::sha1 == to_hash_type( "Sha-1" ) );
        CHECK( hash::type::sha256 == to_hash_type( "sHA-256" ) );
        CHECK( hash::type::sha384 == to_hash_type( "SHa-384" ) );
        CHECK( hash::type::sha512 == to_hash_type( "sha-512" ) );
      }


      SECTION( "random strings yield hash_type_unknown" )
      {
        CHECK( hash::type::unknown == to_hash_type( "" ) );
        CHECK( hash::type::unknown == to_hash_type( "sha" ) );
        CHECK( hash::type::unknown == to_hash_type( "sha2" ) );
        CHECK( hash::type::unknown == to_hash_type( "?" ) );
      }


      SECTION( "to_string yields expected result" )
      {
        CHECK( std::string( "unknown" ) == to_string( hash::type::unknown ) );
        CHECK( std::string( "md4" ) == to_string( hash::type::md4 ) );
        CHECK( std::string( "md5" ) == to_string( hash::type::md5 ) );
        CHECK( std::string( "sha-1" ) == to_string( hash::type::sha1 ) );
        CHECK( std::string( "sha-256" ) == to_string( hash::type::sha256 ) );
        CHECK( std::string( "sha-384" ) == to_string( hash::type::sha384 ) );
        CHECK( std::string( "sha-512" ) == to_string( hash::type::sha512 ) );
      }
    }


    // -------------------------------------------------------------------------------------------------------

    TEST_CASE( "hash_generator is moveable" )
    {
      std::string input = "this is a test string";
      std::string expected = "486eb65274adb86441072afa1e2289f3";

      auto get_hash_generator = [input]() -> hash_generator
      {
        hash_generator g( hash::type::md5 );
        g.add_data( reinterpret_cast<const uint8_t*>( input.data() ), input.size() );
        return g;
      };


      SECTION( "hash_generator is move-constructible" )
      {        
        hash_generator h( std::move( get_hash_generator() ) );
        CHECK( expected == h.retrieve_hash().string );
      }


      SECTION( "hash_generator is move-assignable" )
      {
        hash_generator h( hash::type::md4 );
        h = std::move( get_hash_generator() );
        CHECK( expected == h.retrieve_hash().string );
      }
    }

    
    // -------------------------------------------------------------------------------------------------------

    TEST_CASE( "hash_generator calculates correct hashes" )
    {
      random_block_size randomBlockSize;

      SECTION( "empty string input" )
      {
        std::string input = "";
        std::map< hash::type, std::string > expectedHashes = { 
          { hash::type::md4, "31d6cfe0d16ae931b73c59d7e0c089c0" },
          { hash::type::md5, "d41d8cd98f00b204e9800998ecf8427e" },
          { hash::type::sha1, "da39a3ee5e6b4b0d3255bfef95601890afd80709" },
          { hash::type::sha256, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855" },
          { hash::type::sha512,
          "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d"
          "85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e" } 
        };

        for ( const auto& hash : expectedHashes )
        {
          hash::config cfg;
          cfg.processingBlockSize = randomBlockSize.get();

          auto result = get_hash( input, hash.first, cfg );

          CHECK( hash.first == result.hashType );
          CHECK( hash.second == result.string );
          CHECK( input.length() == result.inputSize );
        }
      }


      SECTION( "short string input" )
      {
        std::string input = "this is a test string";
        std::map< hash::type, std::string > expectedHashes = { 
          { hash::type::md4, "15c6377f90f7a15b6ab1d7d9b306caec" },
          { hash::type::md5, "486eb65274adb86441072afa1e2289f3" },
          { hash::type::sha1, "9a375f77abb15794900c2689812204273d757c9b" },
          { hash::type::sha256, "f6774519d1c7a3389ef327e9c04766b999db8cdfb85d1346c471ee86d65885bc" },
          { hash::type::sha384,
          "f3a9019826c97a784c14a9c59be2c9940675a9312108cbede0ae2d686c0ae4b4f19b388414"
          "82120b97f0dfa49ec5680c" },
          { hash::type::sha512,
          "c240dd0b1a9b00c2478ab95f2184c81d0f3f923a751c71e61af36bb34fe9f240399ca3af2f"
          "061cbc1da2535ce93f6bcedead90cad16f14346cd34f394ee02f5e" } 
        };

        for ( const auto& hash : expectedHashes )
        {
          hash::config cfg;
          cfg.processingBlockSize = randomBlockSize.get();

          auto result = get_hash( input, hash.first, cfg );

          CHECK( hash.first == result.hashType );
          CHECK( hash.second == result.string );
          CHECK( input.length() == result.inputSize );
        }
      }


      SECTION( "long string input" )
      {
        std::string input =
#include "../data/test.string"
        ;
        std::map< hash::type, std::string > expectedHashes = {
          { hash::type::md4, "d134eb0ccf31507e0ffbe56c141310a8" },
          { hash::type::md5, "4896aceea90115e5a0e7b04386168650" },
          { hash::type::sha1, "8b20e600dd9779d66d52293367a237ecb93c833c" },
          { hash::type::sha256, "dd03ec99b67bdb4ec137f8a8549487747ffcbd86c9a3e02c4120a9804c9cec2f" },
          { hash::type::sha384,
          "d09952eddc20092c4f9f6255ea086bccb795abd1f535724cdbf662cbef6833f5b379ca9eb6"
          "353d588b875f69740d481d" },
          { hash::type::sha512,
          "b8c65ff5961e47d09d23bb055092aa56bb30e45bd1399227d66bdfb2b5b22f02725841479e"
          "7e540738ab5a7c4622c94eb7a13d83056a10231f1df7641462321b" }
        };

        for ( const auto& hash : expectedHashes )
        {
          hash::config cfg;
          cfg.processingBlockSize = randomBlockSize.get();

          auto result = get_hash( input, hash.first, cfg );

          CHECK( hash.first == result.hashType );
          CHECK( hash.second == result.string );
          CHECK( input.length() == result.inputSize );
        }
      }


      SECTION( "stream input" )
      {
        auto p = get_executable_path();
        p /= "/data/test.jpg";

        std::uint64_t expectedFileSize = 3184393;

        std::map< hash::type, std::string > expectedHashes = {
          { hash::type::md4, "650bf7c77fe3e65ab81c79e4d8ae42fd" },
          { hash::type::md5, "858760a184d74dec43cb6a5eee5bb551" },
          { hash::type::sha1, "8e76eebc245103a1da51a9a32b5a9fb99d206b33" },
          { hash::type::sha256, "f7de7129fed4c37eb0c74e56d760e9ca69831f84ab0b6f919310ba86b6ab6045" },
          { hash::type::sha384,
          "e08eaa014a5c5d5830086815d119a1f0fd8a3356cd19b9efff6139c04047008e88de0e085d"
          "be8db1ed462ff0f634a1db" },
          { hash::type::sha512,
          "4dcf8be1d491844ca2267756bf70881d288e005a578efd1232cfba7eb65cab84d277e7c2ac"
          "4c4429698ea0a3e900c42356822e997579e0668a84da8295ce141a" } 
        };


        for ( const auto& hash : expectedHashes )
        {
          boost::filesystem::ifstream input( p, std::ios::in | std::ios::binary );

          REQUIRE( input.is_open() );

          hash::config cfg;
          cfg.processingBlockSize = randomBlockSize.get();

          auto result = get_hash( input, hash.first, cfg );

          CHECK( hash.first == result.hashType );
          CHECK( hash.second == result.string );
          CHECK( expectedFileSize == result.inputSize );
        }
      }


      SECTION( "different string casing" )
      {
        std::string input1 = "the test input";
        std::string input2 = "The test input";

        std::vector< hash::type > hashTypes = {
          hash::type::md4,
          hash::type::md5,
          hash::type::sha1,
          hash::type::sha256,
          hash::type::sha384,
          hash::type::sha512
        }; 
        
        for ( auto type : hashTypes )
        {
          auto result1 = get_hash( input1, type );
          auto result2 = get_hash( input2, type );

          CHECK( result1.string != result2.string );
        }
      }
    }

   
    // -------------------------------------------------------------------------------------------------------

    TEST_CASE( "exception behaviour" )
    {
      std::string input = "this is a test string";
      std::string expected = "486eb65274adb86441072afa1e2289f3";

      hash_generator g( hash::type::md5 );

      SECTION( "calling retrieve multiple times yields exception" )
      {
        g.add_data( reinterpret_cast<const uint8_t*>( input.data() ), input.size() );
        g.retrieve_hash();
        CHECK_THROWS_AS( g.retrieve_hash(), crypto::exception );
      }


      SECTION( "calling add_data after retrieve_hash yields exception" )
      {
        auto data = reinterpret_cast<const uint8_t*>( input.data() );

        g.add_data( data, input.size() );
        g.retrieve_hash();
        CHECK_THROWS_AS( g.add_data( data, input.size() ), crypto::exception );
      }


      SECTION( "invalid hash_type yields exception for string" )
      {
        std::string input = "this is a test string";
        CHECK_THROWS_AS( get_hash( input, hash::type::unknown ), exception );
      }


      SECTION( "invalid hash_type yields exception for stream" )
      {
        auto p = get_executable_path();
        p /= "/data/test.jpg";
        boost::filesystem::ifstream input( p, std::ios::in | std::ios::binary );
        CHECK_THROWS_AS( get_hash( input, hash::type::unknown ), exception );
      }


      SECTION( "invalid buffer yields exception" )
      {
        std::vector< hash::type > hashTypes = {
          hash::type::md4,
          hash::type::md5,
          hash::type::sha1,
          hash::type::sha256,
          hash::type::sha384,
          hash::type::sha512
        };
        for ( auto type : hashTypes )
        {
          CHECK_THROWS_AS( get_hash( nullptr, 42, type ), exception );
        }
      }
    }




  }  // namespace test
}  // namespace crypto
}  // namespace ll
