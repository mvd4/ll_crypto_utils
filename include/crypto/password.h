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

#include <string>

#include "crypto/hash.h"


namespace ll
{
namespace crypto
{
  struct pbk
  {
    struct config
    {
      size_t numIterations = 1000;  //!< number of iterations for pbkdf2 algorithm
      size_t outputLength = 32;     //!< length of the string output
    };


    std::string string;                  //!< the string representation
    std::vector< std::uint8_t > binary;  //!< the binary representation
  };


  pbk pbkdf2( const std::string& password_,
              const std::string& salt_,
              hash::type type_ = hash::type::sha256,
              pbk::config cfg_ = pbk::config() );

}  // namespace crypto
}  // namespace ll
