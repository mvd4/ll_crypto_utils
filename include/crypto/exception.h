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

#include <exception>
#include <string>

#include "../support/environment.h"

namespace ll
{
namespace crypto
{
  enum class error
  {
    invalid_parameter,
    invalid_request,
    internal,
  };


  class exception : public std::exception
  {
  public:
    exception( error err_, const std::string detail_ = std::string() );
    exception( error err_, int detail_ );

    error get_error() const LL_NOEXCEPT { return m_error; }

    const char* what() const LL_NOEXCEPT override { return m_message.c_str(); }

  private:
    error m_error;
    std::string m_message;
  };


}  // namespace crypto
}  // namespace ll
