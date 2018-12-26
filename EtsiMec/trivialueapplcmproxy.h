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
