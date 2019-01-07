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

#include "appcharcs.h"

#include "Rest/utils.h"
#include "Support/thrower.h"

namespace uiiit {
namespace etsimec {

AppCharcs::AppCharcs(const unsigned int aMemory,
                     const unsigned int aStorage,
                     const unsigned int aLatency,
                     const unsigned int aBandwidth,
                     const ServiceCont  aServiceCont)
    : rest::DataType() {
  theObject["memory"]      = web::json::value(aMemory);
  theObject["storage"]     = web::json::value(aStorage);
  theObject["latency"]     = web::json::value(aLatency);
  theObject["bandwidth"]   = web::json::value(aBandwidth);
  theObject["serviceCont"] = web::json::value(static_cast<int>(aServiceCont));
}

AppCharcs::AppCharcs()
    : AppCharcs(0, 0, 0, 0, ServiceCont::NOT_REQUIRED) {
}

AppCharcs::AppCharcs(const web::json::value& aJson)
    : DataType(aJson) {
  support::Thrower myThrower("Invalid AppCharcs: ");
  rest::notNumberIfPresent(aJson, "memory", myThrower);
  rest::notNumberIfPresent(aJson, "storage", myThrower);
  rest::notNumberIfPresent(aJson, "latency", myThrower);
  rest::notNumberIfPresent(aJson, "bandwidth", myThrower);
  rest::notNumberIfPresent(aJson, "serviceCont", myThrower);
  if (aJson.has_number_field("serviceCont")) {
    const auto myServiceCont = aJson.at("serviceCont").as_integer();
    myThrower(myServiceCont != static_cast<int>(ServiceCont::NOT_REQUIRED) and
                  myServiceCont != static_cast<int>(ServiceCont::REQUIRED),
              "invalid serviceCont value");
  }
  myThrower.check();
}

} // namespace etsimec
} // namespace uiiit
