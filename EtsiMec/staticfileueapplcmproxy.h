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

#include "EtsiMec/staticueapplcmproxy.h"

namespace uiiit {
namespace etsimec {

/**
 * A static UE application LCM proxy that reads the association between
 * addresses and egde routers from a text file, formatted as:
 *
 * address1 edgerouter1
 * address2 edgerouter2
 * ...
 * addressN edgerouterN
 *
 * The association above cannot be modified at run-time, unless the interface of
 * the base class is used. There is no default edge router.
 */
class StaticFileUeAppLcmProxy : public StaticUeAppLcmProxy
{
 public:
  /**
   * \param aApiRoot The apiRoot assigned.
   * \param aConfFile The input configuration file.
   *
   * \throw std::runtime_error if the configuration file is ill-formed or
   * unreadable or it contains no associations.
   */
  explicit StaticFileUeAppLcmProxy(const std::string& aApiRoot,
                                   const std::string& aConfFile);
};

} // namespace etsimec
} // namespace uiiit
