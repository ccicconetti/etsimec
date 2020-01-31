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

#include "invoker.h"

#include "Rest/client.h"

#include <glog/logging.h>

#include <stdexcept>

namespace uiiit {
namespace wsk {

Invoker::Invoker(const std::string& aApiRoot, const std::string& aAuth)
    : theApiRoot(aApiRoot)
    , thePath("/api/v1/namespaces/_/actions/")
    , theQuery("blocking=true&result=true")
    , theAuth("Basic " + aAuth) {
  if (aApiRoot.empty()) {
    throw std::runtime_error("Invalid empty OpenWhisk API root");
  }
  if (aAuth.empty()) {
    throw std::runtime_error("Invalid empty OpenWhisk auth token");
  }
  VLOG(1) << "created an invoker towards " << aApiRoot;
}

std::pair<bool, std::string> Invoker::
                             operator()(const std::string&                       aName,
           const std::map<std::string, std::string> aParams) const noexcept {
  std::pair<bool, std::string> ret({false, std::string()});
  try {
    rest::Client myClient(theApiRoot, true);
    myClient.changeHeader("Authorization", theAuth);
    auto  myValue  = web::json::value::object();
    auto& myObject = myValue.as_object();
    for (const auto& elem : aParams) {
      myObject[elem.first] = web::json::value(elem.second);
    }
    const auto res = myClient.post(myValue, thePath + aName, theQuery);

    if (res.first != web::http::status_codes::OK) {
      ret.second = "unexpected HTTP response: " + std::to_string(res.first);

    } else if (not res.second.has_string_field("payload")) {
      ret.second = "unexpected body: payload field missing";

    } else {
      ret = std::make_pair(true, res.second.at("payload").as_string());
    }

  } catch (const std::exception& aErr) {
    ret.second = std::string("error encountered: ") + aErr.what();

  } catch (...) {
    ret.second = "unknown error encountered";
  }
  return ret;
}

std::pair<bool, std::string> Invoker::operator()(const std::string& aName) const
    noexcept {
  return this->operator()(aName, {});
}

} // namespace wsk
} // namespace uiiit
