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

#include "applistclient.h"

#include "EtsiMec/utils.h"
#include "Rest/client.h"

namespace uiiit {
namespace etsimec {

AppListClient::AppListClient(const std::string& aApiRoot)
    : theApiRoot(aApiRoot) {
}

AppListClient::ListType AppListClient::operator()() const {
  rest::Client myClient(etsimec::mx2root(theApiRoot));
  const auto   ret = myClient.get("/app_list");
  if (ret.first != web::http::status_codes::OK or
      not ret.second.has_object_field("ApplicationList") or
      not ret.second.at("ApplicationList").has_array_field("appInfo")) {
    throw std::runtime_error("Invalid response from server: " +
                             std::to_string(ret.first));
  }

  ListType myApplications;
  for (const auto& elem :
       ret.second.at("ApplicationList").at("appInfo").as_array()) {
    myApplications.emplace_back(AppInfo(elem));
  }
  return myApplications;
}

} // namespace etsimec
} // namespace uiiit
