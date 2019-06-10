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

#include "appinfo.h"

#include "Rest/utils.h"

#include <stdexcept>

namespace uiiit {
namespace etsimec {

AppInfo::AppInfo(const std::string& aAppName,
                 const std::string& aAppProvider,
                 const std::string& aAppSoftVersion,
                 const std::string& aAppDescription,
                 const std::string& aAppPackageSource)
    : DataType() {
  support::Thrower myThrower("Invalid AppInfo: ");
  commonSetup(
      aAppName, aAppProvider, aAppSoftVersion, aAppDescription, myThrower);
  myThrower.check();

  // set internal fields
  rest::setIfNotEmpty(theObject, "appPackageSource", aAppPackageSource, false);
}

AppInfo::AppInfo(const std::string& aAppName,
                 const std::string& aAppProvider,
                 const std::string& aAppSoftVersion,
                 const std::string& aAppDescription,
                 const std::string& aReferenceUri,
                 const std::string& aAppPackageSource)
    : DataType() {
  support::Thrower myThrower("Invalid AppInfo: ");
  myThrower(aReferenceUri.empty(), "referenceURI missing");
  commonSetup(
      aAppName, aAppProvider, aAppSoftVersion, aAppDescription, myThrower);
  myThrower.check();

  // set internal fields
  rest::setIfNotEmpty(theObject, "referenceURI", aReferenceUri, false);
  rest::setIfNotEmpty(theObject, "appPackageSource", aAppPackageSource, false);
}

AppInfo::AppInfo(const std::string& aAppName,
                 const std::string& aAppProvider,
                 const std::string& aAppSoftVersion,
                 const std::string& aAppDescription,
                 const AppCharcs&   aAppCharcs)
    : DataType() {
  support::Thrower myThrower("Invalid AppInfo: ");
  commonSetup(
      aAppName, aAppProvider, aAppSoftVersion, aAppDescription, myThrower);
  myThrower.check();

  // set internal fields
  theObject["appCharcs"] = aAppCharcs.toJson();
}

void AppInfo::commonSetup(const std::string& aAppName,
                          const std::string& aAppProvider,
                          const std::string& aAppSoftVersion,
                          const std::string& aAppDescription,
                          support::Thrower&  aThrower) {
  // perform all consistency checks
  aThrower(aAppName.empty(), "appName missing");
  aThrower(aAppProvider.empty(), "appProvider missing");
  aThrower(aAppName.size() > 32, "appName exceeds 32 characters");
  aThrower(aAppProvider.size() > 32, "appProvider exceeds 32 characters");
  aThrower(aAppSoftVersion.size() > 32, "appSoftVersion exceeds 32 characters");
  aThrower(aAppDescription.size() > 128,
           "appDescription exceeds 128 characters");

  // fill internal JSON object
  rest::setIfNotEmpty(theObject, "appName", aAppName, false);
  rest::setIfNotEmpty(theObject, "appProvider", aAppProvider, false);
  rest::setIfNotEmpty(theObject, "appSoftVersion", aAppSoftVersion, false);
  rest::setIfNotEmpty(theObject, "appDescription", aAppDescription, false);
}

AppInfo::AppInfo(const web::json::value& aJson)
    : DataType(aJson) {
  support::Thrower myThrower("Invalid AppInfo: ");
  rest::notEmpty(aJson, "appName", myThrower);
  rest::notEmpty(aJson, "appProvider", myThrower);
  rest::tooLong(aJson, "appName", 32, myThrower);
  rest::tooLong(aJson, "appProvider", 32, myThrower);
  rest::tooLong(aJson, "appSoftVersion", 32, myThrower);
  rest::tooLong(aJson, "appDescription", 128, myThrower);
  myThrower(aJson.has_string_field("referenceURI") and
                aJson.has_object_field("appCharcs"),
            "referenceURI and appCharcs cannot be both present in AppInfo");
  if (aJson.has_string_field("referenceURI")) {
    rest::notEmpty(aJson, "referenceURI", myThrower);
  }
  myThrower.check();

  if (aJson.has_object_field("appCharcs")) {
    std::ignore = AppCharcs(aJson.at("appCharcs"));
  }
}

AppInfo
AppInfo::makeAppContextResponse(const std::string& aReferenceUri) const {
  if (message() != Message::AppContextRequest) {
    throw std::runtime_error("Invalid AppContext structure type");
  }
  if (aReferenceUri.empty()) {
    throw std::runtime_error("referenceURI cannot be empty");
  }
  return AppInfo(appName(),
                 appProvider(),
                 appSoftVersion(),
                 appDescription(),
                 aReferenceUri,
                 appPackageSource());
}

AppInfo
AppInfo::makeAppContextRequest(const std::string& aAppPackageSource) const {
  if (message() != Message::ApplicationList) {
    throw std::runtime_error("Invalid ApplicationList structure type");
  }
  return AppInfo(appName(),
                 appProvider(),
                 appSoftVersion(),
                 appDescription(),
                 aAppPackageSource);
}

std::string AppInfo::appName() const {
  assert(theObject.has_string_field("appName"));
  return theObject.as_object().at("appName").as_string();
}

std::string AppInfo::appProvider() const {
  assert(theObject.has_string_field("appProvider"));
  return theObject.as_object().at("appProvider").as_string();
}

std::string AppInfo::appSoftVersion() const {
  return rest::stringOrEmpty(theObject, "appSoftVersion");
}

std::string AppInfo::appDescription() const {
  return rest::stringOrEmpty(theObject, "appDescription");
}

std::string AppInfo::appPackageSource() const {
  return rest::stringOrEmpty(theObject, "appPackageSource");
}

std::string AppInfo::referenceUri() const {
  if (message() != Message::AppContextResponse) {
    throw std::runtime_error(
        "This message type does not have a referenceURI field");
  }
  assert(theObject.has_string_field("referenceURI"));
  return theObject.as_object().at("referenceURI").as_string();
}

AppInfo::Message AppInfo::message() const {
  if (theObject.has_object_field("appCharcs")) {
    assert(not theObject.has_string_field("referenceURI"));
    return Message::ApplicationList;
  } else if (theObject.has_string_field("referenceURI")) {
    return Message::AppContextResponse;
  }
  return Message::AppContextRequest;
}

std::string toString(const AppInfo::Message aMessage) {
  if (aMessage == AppInfo::Message::AppContextRequest) {
    return "AppContext (request)";
  } else if (aMessage == AppInfo::Message::AppContextResponse) {
    return "AppContext (response)";
  } else if (aMessage == AppInfo::Message::ApplicationList) {
    return "ApplicationList";
  }
  return "unknown";
}

} // namespace etsimec
} // namespace uiiit

std::ostream& operator<<(std::ostream&                  aStream,
                         const uiiit::etsimec::AppInfo& aAppInfo) {
  aStream << "appName " << aAppInfo.appName() << ", "
          << "appProvider " << aAppInfo.appProvider() << ", "
          << "appSoftVersion " << aAppInfo.appSoftVersion() << ", "
          << "appDescription " << aAppInfo.appDescription() << ", "
          << "appPackageSource " << aAppInfo.appPackageSource() << ", "
          << uiiit::etsimec::toString(aAppInfo.message());
  return aStream;
}
