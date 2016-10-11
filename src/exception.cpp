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

#include "crypto/exception.h"

#include <map>


namespace ll
{
namespace crypto
{

  namespace
  {
    std::string to_string( error err_ )
    {
      std::string result = "ll::crypto: ";

      switch ( err_ )
      {
        case error::invalid_parameter:
          result += "Invalid parameter";
          break;
        case error::internal:
          result += "Internal error";
        default:
          result += "unknown error;";
      }

      return result;
    }
  }


  // ---------------------------------------------------------------------------------------------------------

  exception::exception( error err_, const std::string detail_ )
    : m_error( err_ ), m_message( to_string( err_ ) )
  {
    if ( !detail_.empty() )
      m_message += " (" + detail_ + ")";
  }


  // ---------------------------------------------------------------------------------------------------------

  exception::exception( error err_, int detail_ )
    : m_error( err_ ), m_message( to_string( err_ ) + " (" + std::to_string( detail_ ) + ")" )
  {
  }


}  // namespace crypto
}  // namespace ll
