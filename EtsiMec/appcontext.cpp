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

#include "appcontext.h"

#include "Rest/utils.h"
#include "Support/thrower.h"

namespace uiiit {
namespace etsimec {

AppContext::AppContext(const std::string& aAssociateUeAppId,
                       const std::string& aCallbackReference,
                       const AppInfo&     aAppInfo)
    : AppContext(
          "", aAssociateUeAppId, aCallbackReference, aAppInfo, Type::Request) {
}

AppContext::AppContext(const std::string& aContextId,
                       const std::string& aAssociateUeAppId,
                       const std::string& aCallbackReference,
                       const AppInfo&     aAppInfo)
    : AppContext(aContextId,
                 aAssociateUeAppId,
                 aCallbackReference,
                 aAppInfo,
                 Type::Response) {
}

AppContext::AppContext(const std::string& aContextId,
                       const std::string& aAssociateUeAppId,
                       const std::string& aCallbackReference,
                       const AppInfo&     aAppInfo,
                       const Type         aType) {
  // perform all consistency checks
  support::Thrower myThrower("Invalid AppContext: ");
  myThrower(aType == Type::Response and aContextId.empty(),
            "contextId missing");
  myThrower(aAssociateUeAppId.empty(), "associateUeAppId missing");
  myThrower((aType == Type::Request and
             aAppInfo.message() != AppInfo::Message::AppContextRequest) or
                (aType == Type::Response and
                 aAppInfo.message() != AppInfo::Message::AppContextResponse),
            "wrong appInfo structure used");
  myThrower.check();

  // fill internal JSON object
  rest::setIfNotEmpty(theObject, "contextId", aContextId, false);
  rest::setIfNotEmpty(theObject, "associateUeAppId", aAssociateUeAppId, false);
  rest::setIfNotEmpty(
      theObject, "callbackReference", aCallbackReference, false);
  theObject["appInfo"] = aAppInfo.toJson();
}

AppContext::AppContext(const web::json::value& aJson)
    : DataType(aJson) {
  support::Thrower myThrower("Invalid AppContext: ");
  rest::notEmptyIfPresent(aJson, "contextId", myThrower);
  rest::notEmpty(aJson, "associateUeAppId", myThrower);
  myThrower(not aJson.has_object_field("appInfo"), "missing appInfo");
  myThrower.check();
  std::ignore = AppInfo(aJson.at("appInfo"));
}

AppContext AppContext::makeResponse(const std::string& aContextId,
                                    const std::string& aReferenceUri) const {
  if (type() != Type::Request) {
    throw std::runtime_error("Invalid AppContext structure type");
  }
  if (aContextId.empty()) {
    throw std::runtime_error("contextId cannot be empty");
  }
  if (aReferenceUri.empty()) {
    throw std::runtime_error("referenceURI cannot be empty");
  }
  return AppContext(aContextId,
                    associateUeAppId(),
                    callbackReference(),
                    appInfo().makeAppContextResponse(aReferenceUri));
}

std::string AppContext::contextId() const {
  return rest::stringOrEmpty(theObject, "contextId");
}

std::string AppContext::associateUeAppId() const {
  const auto ret = rest::stringOrEmpty(theObject, "associateUeAppId");
  assert(not ret.empty());
  return ret;
}

std::string AppContext::callbackReference() const {
  return rest::stringOrEmpty(theObject, "callbackReference");
}

AppInfo AppContext::appInfo() const {
  assert(theObject.has_object_field("appInfo"));
  return AppInfo(theObject.at("appInfo"));
}

AppContext::Type AppContext::type() const {
  assert((theObject.has_string_field("contextId") == Type::Request and
          appInfo().message() == AppInfo::AppContextRequest) or
         (theObject.has_string_field("contextId") == Type::Response and
          appInfo().message() == AppInfo::AppContextResponse));
  return theObject.has_string_field("contextId") ? Type::Response :
                                                   Type::Request;
}

} // namespace etsimec
} // namespace uiiit
