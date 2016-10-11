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

#include <crypto/password.h>
#include <crypto/exception.h>


namespace ll
{
namespace crypto
{
  namespace test
  {

    TEST_CASE( "pbkdf2" )
    {
      SECTION( "yields expected results" )
      {
        std::string input = "TestPasswordWith#Numbers123";
        std::string salt = "TheSalT";

        std::map< hash::type, std::string > expectedHashStrings =
        {
#if !LL_IS_OSX()
          { hash::type::md4, "705dbe2ef52e903e4216dd1730d51ec4" },
          { hash::type::md5, "9a7ce9b7fd9657e325f46966e4242903" },
#endif
          { hash::type::sha1, "3b0a50f55c2d2b4822fed63e5890120d" },
          { hash::type::sha256, "56bf951b15e0b6886813170029e00934" },
          { hash::type::sha384, "817a47ca56e231fe533c1861f3dea28d" },
          { hash::type::sha512, "2817cbb27cd8549bcbdac982b2421ae6" }
        };

        for ( const auto& hash : expectedHashStrings )
        {
          auto result = pbkdf2( input, salt, hash.first );
          CHECK( hash.second == result.string );
        }
      }

    
      // -------------------------------------------------------------------------------------------------------

      SECTION( "throws on empty password" )
      {
        std::string input;
        std::string salt = "TheSalT";

        CHECK_THROWS_AS( pbkdf2( input, salt, hash::type::sha256 ), crypto::exception );
      }


      // -------------------------------------------------------------------------------------------------------

      SECTION( "throws on invalid hash type" )
      {
        std::string input = "TestPasswordWith#Numbers123";
        std::string salt = "TheSalT";

        CHECK_THROWS_AS( pbkdf2( input, salt, hash::type::unknown ), crypto::exception );
      }


      // -------------------------------------------------------------------------------------------------------

      SECTION( "throws on invalid cfg" )
      {
        std::string input = "TestPasswordWith#Numbers123";
        std::string salt = "TheSalT";
      
        {
          pbk::config cfg;
          cfg.numIterations = 0;

          CHECK_THROWS_AS( pbkdf2( input, salt, hash::type::sha256, cfg ), crypto::exception );
        }

        {
          pbk::config cfg;
          cfg.outputLength = 0;

          CHECK_THROWS_AS( pbkdf2( input, salt, hash::type::sha256, cfg ), crypto::exception );
        }
      }
    }

  }  // namespace test
}  // namespace crypto
}  // namespace ll
