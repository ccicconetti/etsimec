/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ ETSI MEC library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/etsimec/
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

#include "EtsiMec/staticueapplcmproxy.h"

#include <set>
#include <string>

namespace uiiit {
namespace etsimec {

class TrivialStaticUeAppLcmProxy final : public StaticUeAppLcmProxy
{
 public:
  explicit TrivialStaticUeAppLcmProxy(const std::string&           aApiRoot,
                                      const std::set<std::string>& aAppNames)
      : StaticUeAppLcmProxy(aApiRoot)
      , theCounter(0) {
    for (const auto& myAppName : aAppNames) {
      addApp(AppInfo(
          myAppName, "provider", "version", "description", AppCharcs()));
    }
  }

  bool add(const std::string& aAddress, const std::string& aName) {
    ++theCounter;
    const auto myResponse =
        createContext(aAddress,
                      AppContext(std::to_string(theCounter),
                                 "",
                                 AppInfo(aName,
                                         "provider",
                                         "version",
                                         "description",
                                         "package source")));
    return myResponse.type() == AppContext::Type::Response;
  }

  bool contextExists(const std::string& aAddress,
                     const std::string& aAppName,
                     const std::string& aEdgeRouter) const {
    for (const auto& elem : contexts()) {
      if (std::get<0>(elem) == aAddress and std::get<1>(elem) == aAppName and
          std::get<2>(elem) == aEdgeRouter) {
        return true;
      }
    }
    return false;
  }

 private:
  size_t theCounter;
};

} // namespace etsimec
} // namespace uiiit
