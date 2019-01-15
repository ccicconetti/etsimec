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

#include "staticueapplcmproxy.h"

#include "EtsiMec/notificationevent.h"
#include "Rest/client.h"
#include "Support/uuid.h"

#include <glog/logging.h>

namespace uiiit {
namespace etsimec {

StaticUeAppLcmProxy::StaticUeAppLcmProxy(const std::string& aApiRoot)
    : UeAppLcmProxy(aApiRoot)
    , theMutex()
    , theTable()
    , theApplicationsByContextId()
    , theApplicationsByUeAppId() {
}

void StaticUeAppLcmProxy::associateAddress(const std::string& aAddress,
                                           const std::string& aAppName,
                                           const std::string& aEdgeRouter) {
  if (aAppName.empty()) {
    throw std::runtime_error("Empty application name");
  }
  if (aEdgeRouter.empty()) {
    throw std::runtime_error("Empty edge router end-point");
  }

  const auto myKey = AddressAppKey(aAddress, aAppName);

  std::unique_lock<std::mutex> myLock(theMutex);
  auto                         it = theTable.emplace(myKey, aEdgeRouter);
  if (not it.second and aEdgeRouter == it.first->second) {
    VLOG(2) << "requesting same association between <" << aAddress << ", "
            << aAppName << "> and " << aEdgeRouter << ": ignored";
    return;
  }
  VLOG(1) << "associated <" << aAddress << ", " << aAppName << "> to "
          << aEdgeRouter;
  it.first->second = aEdgeRouter;

  // update all descriptors whose target edge router has changed
  NotificationList myNotifications;
  for (auto& elem : theApplicationsByContextId) {
    if (elem.second.theAppName != aAppName) {
      continue;
    }

    const auto it = find(elem.second.theClientAddress, elem.second.theAppName);
    assert(it != theTable.end());

    if (it->second != elem.second.theEdgeRouterEndpoint) {
      myNotifications.emplace_back(std::make_tuple(
          elem.second.theCallbackReference, aEdgeRouter, elem.first));
      elem.second.theEdgeRouterEndpoint = aEdgeRouter;
    }
  }
  myLock.unlock();
  notifyClients(myNotifications);
}

void StaticUeAppLcmProxy::removeAddress(const std::string& aAddress,
                                        const std::string& aAppName) {
  if (aAppName.empty()) {
    throw std::runtime_error("Empty application name");
  }
  const auto myKey = AddressAppKey(aAddress, aAppName);

  std::unique_lock<std::mutex> myLock(theMutex);
  const auto                   it = theTable.find(myKey);
  if (it != theTable.end()) {
    VLOG(1) << "removed association of <" << aAddress << ", " << aAppName
            << "> from " << it->second;
    theTable.erase(it);
  } else {
    LOG(WARNING) << "trying to remove unexisting entry for <" << aAddress
                 << ", " << aAppName << ">: ignored";
  }

  // update all descriptors whose target edge router has changed
  NotificationList myNotifications;
  for (auto& elem : theApplicationsByContextId) {
    if (elem.second.theAppName != aAppName) {
      continue;
    }

    const auto it = find(elem.second.theClientAddress, elem.second.theAppName);

    // it may happen that due the removal of an entry there are no more entries
    // matching a given pair <address, app name>: in this case we simply log
    // the error without notiying the edge client (because we would not know
    // what to notify), but we set the edge router in the descriptor to an
    // empty string so that if ever the pair <address, app name> is given a
    // valid target, at least then the edge client gets notified
    if (it == theTable.end()) {
      LOG(ERROR) << "<" << elem.second.theClientAddress << ", "
                 << elem.second.theAppName
                 << "> has no valid target edge router";
      elem.second.theEdgeRouterEndpoint = "";

    } else if (it->second != elem.second.theEdgeRouterEndpoint) {
      assert(not it->second.empty());
      elem.second.theEdgeRouterEndpoint = it->second;
      myNotifications.emplace_back(std::make_tuple(
          elem.second.theCallbackReference, it->second, elem.first));
    }
  }
  myLock.unlock();
  notifyClients(myNotifications);
}

size_t StaticUeAppLcmProxy::numAddresses() const {
  const std::lock_guard<std::mutex> myLock(theMutex);
  return theTable.size();
}
size_t StaticUeAppLcmProxy::numContexts() const {
  const std::lock_guard<std::mutex> myLock(theMutex);
  assert(theApplicationsByContextId.size() == theApplicationsByUeAppId.size());
  return theApplicationsByContextId.size();
}

std::list<std::tuple<std::string, std::string, std::string>>
StaticUeAppLcmProxy::addressAssociations() const {
  const std::lock_guard<std::mutex>                            myLock(theMutex);
  std::list<std::tuple<std::string, std::string, std::string>> myRet;
  for (const auto& elem : theTable) {
    myRet.emplace_back(std::make_tuple(
        elem.first.theAddress, elem.first.theAppName, elem.second));
  }
  return myRet;
}

AppContext StaticUeAppLcmProxy::createContext(const std::string& aClientAddress,
                                              const AppContext&  aRequest) {
  const std::lock_guard<std::mutex> myLock(theMutex);

  VLOG(1) << "requested context creation by " << aClientAddress << ": "
          << aRequest.appInfo();

  // check if an application matching the request exists
  if (not exists(aRequest.appInfo())) {
    return aRequest;
  }

  // find the edge router associated to the edge client address and app name
  const auto it = find(aClientAddress, aRequest.appInfo().appName());
  if (it == theTable.end()) {
    // no edge router found: return request without assigning a context ID
    return aRequest;
  }

  // assign a unique context ID to this UE application
  const auto myResponse =
      aRequest.makeResponse(support::Uuid().toString(), it->second);

  const auto ret =
      theApplicationsByContextId.emplace(myResponse.contextId(),
                                         Desc{aRequest.associateUeAppId(),
                                              aRequest.callbackReference(),
                                              aClientAddress,
                                              aRequest.appInfo().appName(),
                                              it->second});
  assert(ret.second);

  const auto myAppRet =
      theApplicationsByUeAppId.emplace(aRequest.associateUeAppId(), ret.first);
  assert(myAppRet.second);

  if (VLOG_IS_ON(2)) {
    std::stringstream myStream;
    for (const auto& elem : theApplicationsByContextId) {
      myStream << "context_id " << elem.first << ", ue_app_id "
               << elem.second.theAssociateUeAppId << ", callback "
               << elem.second.theCallbackReference << ", client_address "
               << elem.second.theClientAddress << ", app_name "
               << elem.second.theAppName << ", edge_router "
               << elem.second.theEdgeRouterEndpoint << '\n';
    }
    VLOG(2) << "active contexts:\n" << myStream.str();
  }

  return myResponse;
}

bool StaticUeAppLcmProxy::updateContext(const AppContext& aRequest) {
  const std::lock_guard<std::mutex> myLock(theMutex);

  const auto it = theApplicationsByUeAppId.find(aRequest.associateUeAppId());
  if (it == theApplicationsByUeAppId.end()) {
    return false;
  }

  VLOG(1) << "updated callbackReference of " << aRequest.associateUeAppId()
          << " from " << it->second->second.theCallbackReference << " to "
          << aRequest.callbackReference();
  it->second->second.theCallbackReference = aRequest.callbackReference();

  return true;
}

bool StaticUeAppLcmProxy::deleteContext(const std::string& aContextId) {
  const std::lock_guard<std::mutex> myLock(theMutex);

  auto it = theApplicationsByContextId.find(aContextId);
  if (it == theApplicationsByContextId.end()) {
    return false;
  }

  auto jt = theApplicationsByUeAppId.find(it->second.theAssociateUeAppId);
  assert(jt != theApplicationsByUeAppId.end());

  theApplicationsByContextId.erase(it);
  theApplicationsByUeAppId.erase(jt);

  return true;
}

void StaticUeAppLcmProxy::notifyClients(
    const NotificationList& aNotificationList) {
  ASSERT_IS_NOT_LOCKED(theMutex);
  for (const auto& elem : aNotificationList) {
    const auto& myCallbackReference = std::get<0>(elem);
    const auto& myReferenceUri      = std::get<1>(elem);
    const auto& myContextId         = std::get<2>(elem);
    assert(not myCallbackReference.empty());
    assert(not myReferenceUri.empty());
    assert(not myContextId.empty());
    if (not web::uri::validate(myCallbackReference)) {
      LOG(WARNING) << "invalid callback reference " << myCallbackReference
                   << ": skipping notification";
      continue;
    }

    const auto myLog = std::string("when notifying ") + myCallbackReference +
                       " of new callback reference " + myReferenceUri +
                       " for context id " + myContextId;
    VLOG(2) << "called " << myLog;
    try {
      rest::Client myClient(myCallbackReference);
      const auto   ret =
          myClient.post(NotificationEvent(myReferenceUri).toJson()).first;
      if (ret != web::http::status_codes::NoContent) {
        const auto myPurged = purge(myContextId);
        LOG(WARNING) << "Invalid response obtained " << myLog << ": "
                     << (myPurged ? "" : "not ") << " purged";
      }
    } catch (const std::exception& aErr) {
      LOG(WARNING) << "error " << myLog << ": " << aErr.what();
    } catch (...) {
      LOG(WARNING) << "unknown error " << myLog;
    }
  }
}

bool StaticUeAppLcmProxy::purge(const std::string& aContextId) {
  const std::lock_guard<std::mutex> myLock(theMutex);
  auto it = theApplicationsByContextId.find(aContextId);
  if (it == theApplicationsByContextId.end()) {
    return false;
  }
  auto jt = theApplicationsByUeAppId.find(it->second.theAssociateUeAppId);
  assert(jt != theApplicationsByUeAppId.end());
  if (jt != theApplicationsByUeAppId.end()) {
    theApplicationsByUeAppId.erase(jt);
  }
  theApplicationsByContextId.erase(it);
  assert(theApplicationsByContextId.size() == theApplicationsByUeAppId.size());
  return true;
}

StaticUeAppLcmProxy::Table::const_iterator
StaticUeAppLcmProxy::find(const std::string& aAddress,
                          const std::string& aAppName) const {
  ASSERT_IS_LOCKED(theMutex);
  assert(not aAppName.empty());

  const auto mySpecific = theTable.find(AddressAppKey(aAddress, aAppName));

  if (mySpecific != theTable.end()) {
    return mySpecific;
  }

  return theTable.find(AddressAppKey("", aAppName)); // may be end()
}

} // namespace etsimec
} // namespace uiiit
