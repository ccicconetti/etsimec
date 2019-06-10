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

#include "ueapplcmproxy.h"

#include "EtsiMec/applicationlist.h"
#include "EtsiMec/utils.h"
#include "Rest/problemdetails.h"

#include <glog/logging.h>

namespace uiiit {
namespace etsimec {

const std::string UeAppLcmProxy::theApiName{"mx2"};
const std::string UeAppLcmProxy::theApiVersion{"v1"};

UeAppLcmProxy::UeAppLcmProxy(const std::string& aApiRoot)
    : EtsiMecServer(aApiRoot, theApiName, theApiVersion)
    , theMutex()
    , theApplications() {
  (*this)(web::http::methods::GET,
          "app_list",
          [this](web::http::http_request aReq) { handleAppList(aReq); });
  (*this)(web::http::methods::POST,
          "app_contexts",
          [this](web::http::http_request aReq) { handleContextCreate(aReq); });
  (*this)(web::http::methods::PUT,
          "app_contexts/(.*)",
          [this](web::http::http_request aReq) { handleContextUpdate(aReq); });
  (*this)(web::http::methods::DEL,
          "app_contexts/(.*)",
          [this](web::http::http_request aReq) { handleContextDelete(aReq); });
}

const std::string& UeAppLcmProxy::staticApiName() noexcept {
  return theApiName;
}

const std::string& UeAppLcmProxy::staticApiVersion() noexcept {
  return theApiVersion;
}

const std::string& UeAppLcmProxy::apiName() const noexcept {
  return theApiName;
}

const std::string& UeAppLcmProxy::apiVersion() const noexcept {
  return theApiVersion;
}

bool UeAppLcmProxy::addApp(const AppInfo& aAppInfo) {
  const std::lock_guard<std::mutex> myLock(theMutex);
  const auto                        myKey = mangle(aAppInfo);
  auto ret = theApplications.insert({myKey, aAppInfo});
  if (not ret.second) {
    ret.first->second = aAppInfo;
    return false;
  }
  return true;
}

bool UeAppLcmProxy::delApp(const AppInfo& aAppInfo) {
  const std::lock_guard<std::mutex> myLock(theMutex);
  const auto                        myKey = mangle(aAppInfo);
  return theApplications.erase(myKey) > 0;
}

bool UeAppLcmProxy::exists(const AppInfo& aAppInfo) const {
  const std::lock_guard<std::mutex> myLock(theMutex);
  return theApplications.find(mangle(aAppInfo)) != theApplications.end();
}

std::string UeAppLcmProxy::mangle(const AppInfo& aAppInfo) {
  return aAppInfo.appName() + "+" + aAppInfo.appProvider() + "+" +
         aAppInfo.appSoftVersion();
}

void UeAppLcmProxy::handleAppList(web::http::http_request aReq) const {
  const std::lock_guard<std::mutex> myLock(theMutex);
  web::json::value                  myRet;
  myRet["ApplicationList"] = ApplicationList(theApplications).toJson();
  aReq.reply(web::http::status_codes::OK, myRet);
}

void UeAppLcmProxy::handleContextCreate(web::http::http_request aReq) {
  const auto myResponse =
      createContext(aReq.remote_address(), AppContext(parseJson(aReq)));

  if (myResponse.type() == AppContext::Type::Response) {
    // creation accepted
    assert(not myResponse.contextId().empty());
    assert(not myResponse.appInfo().referenceUri().empty());
    aReq.reply(web::http::status_codes::Created, myResponse.toJson());

  } else {
    // creation refused
    rest::ProblemDetails myProblemDetails(
        web::http::status_codes::Forbidden,
        "Could not create the requested UE application context because of the "
        "current status of the mobile edge system");
    aReq.reply(myProblemDetails.status(), myProblemDetails.toJson());
  }
}

void UeAppLcmProxy::handleContextUpdate(web::http::http_request aReq) {
  const auto mySuccess = updateContext(AppContext(parseJson(aReq)));
  if (mySuccess) {
    aReq.reply(web::http::status_codes::NoContent);
  } else {
    rest::ProblemDetails myProblemDetails(
        web::http::status_codes::Forbidden,
        "Unknown or invalid UE application context");
    aReq.reply(myProblemDetails.status(), myProblemDetails.toJson());
  }
}

void UeAppLcmProxy::handleContextDelete(web::http::http_request aReq) {
  static const std::string myPrefix("app_contexts/");
  auto                     myPath = aReq.relative_uri().path();
  assert(myPath.find(myPrefix) == 0);
  myPath.erase(0, myPrefix.size());
  const auto mySuccess = deleteContext(myPath);
  if (mySuccess) {
    aReq.reply(web::http::status_codes::NoContent);
  } else {
    rest::ProblemDetails myProblemDetails(web::http::status_codes::Forbidden,
                                          "Unknown UE application context");
    aReq.reply(myProblemDetails.status(), myProblemDetails.toJson());
  }
}

} // namespace etsimec
} // namespace uiiit
