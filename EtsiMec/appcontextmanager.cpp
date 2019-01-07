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

#include "appcontextmanager.h"

#include "EtsiMec/appcontext.h"
#include "EtsiMec/utils.h"
#include "Rest/client.h"
#include "Rest/problemdetails.h"
#include "Support/split.h"
#include "Support/uuid.h"

#include <glog/logging.h>

#include <list>

namespace uiiit {
namespace etsimec {

AppContextManager::AppContextManager(const std::string& aNotificationUri,
                                     const std::string& aProxyUri)
    : rest::Server(aNotificationUri)
    , theNotificationUri(aNotificationUri)
    , theProxyUri(aProxyUri)
    , theMutex()
    , theContexts() {
  (*this)(web::http::methods::POST,
          "/(.*)",
          [this](web::http::http_request aReq) { handleNotification(aReq); });
}

AppContextManager::~AppContextManager() {
  for (const auto& elem : theContexts) {
    const auto myContextId = elem.second.theContextId;
    try {
      const auto ret =
          rest::Client(theProxyUri + "/mx2/v1/app_contexts/" + myContextId)
              .del();
      if (ret == web::http::status_codes::NoContent) {
        LOG(INFO) << "Deleted context for app ID " << elem.first;
      } else {
        LOG(WARNING) << "Error when deleting context for app ID " << elem.first
                     << ": server returned " << ret;
      }
    } catch (const std::exception& aErr) {
      LOG(WARNING) << "Exception when deleting context for app ID "
                   << elem.first << ": " << aErr.what();
    } catch (...) {
      LOG(WARNING) << "Unknown exception when deleting context for app ID "
                   << elem.first;
    }
  }
}

std::pair<std::string, std::string>
AppContextManager::contextCreate(const AppInfo& aAppInfo) {
  if (aAppInfo.message() != AppInfo::Message::AppContextRequest) {
    throw std::runtime_error("Invalid AppInfo passed in context creation");
  }
  const auto myUeAppId = support::Uuid().toString(); // new random ID
  const auto ret =
      rest::Client(theProxyUri + "/mx2/v1/app_contexts")
          .post(AppContext(
                    myUeAppId, theNotificationUri + "/" + myUeAppId, aAppInfo)
                    .toJson());
  if (ret.first != web::http::status_codes::Created) {
    throw std::runtime_error(
        "Invalid response from server upon creation of AppContext: " +
        rest::safeProblemDetailsString(ret.second));
  }
  const auto myResponse = AppContext(ret.second);
  const auto myAppInfo  = myResponse.appInfo();
  if (myResponse.type() == AppContext::Type::Response and
      myAppInfo.message() == AppInfo::Message::AppContextResponse) {
    const auto                        myReferenceUri = myAppInfo.referenceUri();
    const std::lock_guard<std::mutex> myLock(theMutex);
    theContexts.emplace(myUeAppId,
                        Desc{myReferenceUri, myResponse.contextId()});
    return std::make_pair(myUeAppId, myReferenceUri);
  }
  throw std::runtime_error(
      "Invalid body in the server response upon creation of AppContext");
}

std::string AppContextManager::referenceUri(const std::string& aUeAppId) const {
  const std::lock_guard<std::mutex> myLock(theMutex);
  const auto                        it = theContexts.find(aUeAppId);
  if (it != theContexts.end()) {
    return it->second.theReferenceUri;
  }
  throw std::runtime_error("Invalid UE application ID " + aUeAppId);
}

void AppContextManager::handleNotification(web::http::http_request aReq) {
  // read the new reference URI from the body
  const auto myJson = parseJson(aReq);

  // infer the UE application ID from the URL
  const auto myTokens = support::split<std::list<std::string>>(
      aReq.relative_uri().to_string(), "/");
  if (not myJson.is_object() or not myJson.has_string_field("referenceURI") or
      myTokens.empty()) {
    throw std::runtime_error("Invalid NotificationEvent structure");
  }

  const auto myUeAppId = myTokens.back();
  assert(not myUeAppId.empty());
  const auto myReferenceUri = myJson.at("referenceURI").as_string();

  // critical section
  const std::lock_guard<std::mutex> myLock(theMutex);
  auto                              it = theContexts.find(myUeAppId);
  if (it != theContexts.end()) {
    VLOG(1) << "Updated referenceURI of app ID " << myUeAppId
            << " with context ID " << it->second.theContextId << " from "
            << it->second.theReferenceUri << " to " << myReferenceUri;
    it->second.theReferenceUri = myReferenceUri;
  } else {
    throw std::runtime_error(
        "Received notification for unknown UE application " + myUeAppId);
  }

  aReq.reply(web::http::status_codes::NoContent);
}

} // namespace etsimec
} // namespace uiiit
