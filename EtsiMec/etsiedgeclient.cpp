#include "etsiedgeclient.h"

#include "Edge/edgeclient.h"
#include "EtsiMec/appcontextmanager.h"
#include "EtsiMec/applistclient.h"

#include "glog/logging.h"

#include <algorithm>

namespace ec = uiiit::edge;

namespace uiiit {
namespace etsimec {

EtsiEdgeClient::EtsiEdgeClient(AppContextManager& aAppContextManager)
    : ec::EdgeClientInterface()
    , theAppContextManager(aAppContextManager)
    , theClients() {
}

EtsiEdgeClient::~EtsiEdgeClient() {
  // nihil
}

ec::LambdaResponse EtsiEdgeClient::RunLambda(const ec::LambdaRequest& aReq,
                                             const bool               aDry) {
  const auto& myLambda = aReq.theName;
  if (myLambda.empty()) {
    throw std::runtime_error("Invalid empty lambda name");
  }

  auto& myClient = find(myLambda);

  return myClient.RunLambda(aReq, aDry);
}

ec::EdgeClient& EtsiEdgeClient::find(const std::string& aLambda) {
  auto it = theClients.find(aLambda);

  // create a context if it does not exist already
  if (it == theClients.end()) {
    LOG(INFO) << "Retrieving list of UE applictions from LCM proxy";
    // retrieve list from LCM proxy
    const auto myApplications =
        AppListClient(theAppContextManager.proxyUri())();

    const auto jt = std::find_if(myApplications.begin(),
                                 myApplications.end(),
                                 [&aLambda](const AppInfo& aAppInfo) {
                                   return aLambda == aAppInfo.appName();
                                 });

    if (jt == myApplications.end()) {
      throw std::runtime_error("UE application " + aLambda +
                               " not found in the LCM proxy");
    }

    // create the context, with an empty appPackageSource
    const auto myContext =
        theAppContextManager.contextCreate(jt->makeAppContextRequest(""));

    LOG(INFO) << "Creating context for " << aLambda << " with AppInfo: " << *jt;
    auto myEmplaceRet = theClients.emplace(
        aLambda,
        Desc(myContext.first,
             myContext.second,
             std::make_unique<ec::EdgeClient>(myContext.second)));
    assert(myEmplaceRet.second);
    it = myEmplaceRet.first; // overrides outer iterator
  }

  assert(it != theClients.end());

  // check if the UE application has been assigned a new reference URI
  const auto myNewReferenceUri =
      theAppContextManager.referenceUri(it->second.theUeAppId);
  if (it->second.theReferenceUri != myNewReferenceUri) {
    // drop the existing edge client and create a new one that directs
    // lambda functions to the latest reference URI
    LOG(INFO) << "Reference URI for " << aLambda << " updated from "
              << it->second.theReferenceUri << " to " << myNewReferenceUri;
    it->second.theReferenceUri = myNewReferenceUri;
    it->second.theClient.reset(new ec::EdgeClient(myNewReferenceUri));
  }

  assert(it->second.theClient.get() != nullptr);

  return *it->second.theClient;
}

} // namespace etsimec
} // namespace uiiit
