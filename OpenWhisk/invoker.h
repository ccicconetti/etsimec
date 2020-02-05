/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ ETSI MEC library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/wsk/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2019 Claudio Cicconetti https://ccicconetti.github.io/

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "OpenWhisk/command.h"

#include <map>
#include <string>

namespace uiiit {
namespace wsk {

/**
 * Invoke an action on an OpenWhisk server.
 */
class Invoker final : public Command
{
 public:
     using Parameters = std::map<std::string, std::string>;

  /**
   * \param aApiRoot The URI of the OpenWhisk server.
   *
   * \param aAuth The authorization token.
   *
   * \throw std::runtime_error if the URI or token are empty.
   */
  explicit Invoker(const std::string& aApiRoot, const std::string& aAuth);

  /**
   * Invoke a blocking action with result, which is returned in case
   * of success.
   *
   * \param aName The action name.
   *
   * \param aParams The parameters.
   *
   * \return a pair containing a flag on whether the action was successful or
   * not and a string representing an explanation of the error (if not
   * successful) or the result of the action (if successful).
   */
  std::pair<bool, std::string>
  operator()(const std::string&                       aName,
             const Parameters aParams) const noexcept;

  //! Invoke with parameters JSON-encoded.
  std::pair<bool, std::string> operator()(const std::string& aName,
                                          const std::string& aParams) const
      noexcept;

  //! Invoke without parameters.
  std::pair<bool, std::string> operator()(const std::string& aName) const
      noexcept;

 private:
  const std::string theQuery;
};

} // namespace wsk
} // namespace uiiit
