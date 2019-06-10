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

#include "EtsiMec/ueapplcmproxy.h"
#include "EtsiMec/appcontext.h"

#include <string>

namespace uiiit {
namespace etsimec {

struct TrivialUeAppLcmProxy final : public UeAppLcmProxy {
  TrivialUeAppLcmProxy(const std::string& aApiRoot)
      : UeAppLcmProxy(aApiRoot)
      , theClientAddress()
      , theUpdated(0)
      , theContextCounter(0)
      , theContextId()
      , theValidRequest(true) {
  }

  AppContext createContext(const std::string& aClientAddress,
                           const AppContext&  aRequest) override {
    if (theValidRequest) {
      theClientAddress = aClientAddress;
      theContextCounter++;
      return aRequest.makeResponse(
          "context-id-" + std::to_string(theContextCounter),
          "reference-uri-" + std::to_string(theContextCounter));
    }
    return aRequest;
  }
  bool updateContext(const AppContext& aRequest) override {
    theUpdated++;
    return theValidRequest;
  }
  bool deleteContext(const std::string& aContextId) override {
    theContextId = aContextId;
    return theValidRequest;
  }

  std::string theClientAddress;
  size_t      theUpdated;
  size_t      theContextCounter;
  std::string theContextId;
  bool        theValidRequest;
};


}
}
