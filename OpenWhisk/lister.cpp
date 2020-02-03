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

#include "lister.h"

#include "Rest/client.h"

#include <glog/logging.h>

#include <stdexcept>

namespace uiiit {
namespace wsk {

Lister::Lister(const std::string& aApiRoot, const std::string& aAuth)
    : Command(aApiRoot, aAuth) {
  // noop
}

std::map<ActionKey, Action> Lister::operator()(const size_t aLimit,
                                               const size_t aSkip) const {
  VLOG(1) << "request to list " << theApiRoot << " with limit " << aLimit
          << ", skip " << aSkip;

  std::map<ActionKey, Action> ret;

  rest::Client myClient(theApiRoot, true);
  myClient.changeHeader("Authorization", theAuth);
  const auto res      = myClient.get(thePath, makeQuery(aLimit, aSkip));

  if (res.first != web::http::status_codes::OK) {
    throw std::runtime_error("unexpected HTTP response: " +
                             std::to_string(res.first));

  } else if (not res.second.is_array()) {
    throw std::runtime_error("unexpected response: not an array");
  }

  for (const auto& elem : res.second.as_array()) {
    if (not elem.is_object() or not elem.has_string_field("name") or
        not elem.has_string_field("namespace") or
        not elem.has_number_field("updated") or
        not elem.has_string_field("version")) {
      throw std::runtime_error("unexpected response: mandatory field missing");
    }
    const Action myAction(elem.at("namespace").as_string(),
                          elem.at("name").as_string(),
                          elem.at("updated").as_number().to_uint64(),
                          elem.at("version").as_string());
    ret.emplace(myAction.key(), myAction);
  }

  return ret;
}

std::string Lister::makeQuery(const size_t aLimit, const size_t aSkip) {
  return std::string("limit=") + std::to_string(aLimit) +
         "&skip=" + std::to_string(aSkip);
}

} // namespace wsk
} // namespace uiiit
