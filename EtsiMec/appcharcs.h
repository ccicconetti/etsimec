/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ ETSI MEC library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/etsimec/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2018 Claudio Cicconetti <https://about.me/ccicconetti>

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

#include "Rest/datatype.h"

namespace uiiit {
namespace etsimec {

/**
 * ETSI MEC application characteristics.
 *
 * See ETSI GS MEC 016 v1.1.1 (2017-09).
 */
class AppCharcs : public rest::DataType
{
 public:
  enum class ServiceCont : int {
    NOT_REQUIRED = 0,
    REQUIRED     = 1,
  };

  explicit AppCharcs(const unsigned int aMemory,
                     const unsigned int aStorage,
                     const unsigned int aLatency,
                     const unsigned int aBandwidth,
                     const ServiceCont  aServiceCont);

  //! A set of empty application characteristics.
  explicit AppCharcs();

  /**
   * Build from JSON.
   *
   * \throw std::runtime_error if invalid.
   */
  explicit AppCharcs(const web::json::value& aJson);
};

} // namespace etsimec
} // namespace uiiit
