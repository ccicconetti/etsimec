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

#include "staticfileueapplcmproxy.h"

#include <fstream>

namespace uiiit {
namespace etsimec {

StaticFileUeAppLcmProxy::StaticFileUeAppLcmProxy(const std::string& aApiRoot,
                                                 const std::string& aConfFile)
    : StaticUeAppLcmProxy(aApiRoot) {
  std::ifstream myInput(aConfFile);
  if (not myInput) {
    throw std::runtime_error("Cannot open " + aConfFile + " for reading");
  }

  auto myEmpty = true;
  while (not myInput.eof()) {
    std::string myAddress;
    std::string myAppName;
    std::string myEdgeRouter;

    myInput >> myAddress;
    if (not myInput) {
      break;
    }
    if (myAddress == "*") {
      myAddress = "";
    }

    myInput >> myAppName;
    if (not myInput) {
      throw std::runtime_error("Configuration file " + aConfFile +
                               " is ill-formed");
    }

    myInput >> myEdgeRouter;
    if (not myInput) {
      throw std::runtime_error("Configuration file " + aConfFile +
                               " is ill-formed");
    }

    assert(not myAppName.empty() and not myEdgeRouter.empty());

    myEmpty = false;
    associateAddress(myAddress, myAppName, myEdgeRouter);
  }

  if (myEmpty) {
    throw std::runtime_error("Empty configuration in " + aConfFile);
  }
}

} // namespace etsimec
} // namespace uiiit
