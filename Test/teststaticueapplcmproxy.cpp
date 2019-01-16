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

#include "gtest/gtest.h"

#include "EtsiMec/appcontext.h"
#include "EtsiMec/appcontextmanager.h"
#include "EtsiMec/staticfileueapplcmproxy.h"
#include "EtsiMec/staticueapplcmproxy.h"
#include "EtsiMec/trivialueapplcmproxy.h"
#include "Rest/client.h"
#include "Support/wait.h"

#include <glog/logging.h>

#include <boost/filesystem.hpp>

#include <map>
#include <thread>

namespace uiiit {
namespace etsimec {

struct TestStaticUeAppLcmProxy : public ::testing::Test {
  TestStaticUeAppLcmProxy()
      : theProxyUri("http://localhost:10000")
      , theProxy(theProxyUri) {
    theProxy.start();
  }

  bool context(const std::string& aAddress,
               const std::string& aAppName,
               const std::string& aEdgeRouter) {
    for (const auto& elem : theProxy.contexts()) {
      if (std::get<0>(elem) == aAddress and std::get<1>(elem) == aAppName and
          std::get<2>(elem) == aEdgeRouter) {
        return true;
      }
    }
    return false;
  }

  const std::string   theProxyUri;
  StaticUeAppLcmProxy theProxy;
};

TEST_F(TestStaticUeAppLcmProxy, test_appcontextmanager_notifications) {
  const std::string myNotificationUri = "http://localhost:10001";
  theProxy.start();
  AppContextManager myAppContextManager(myNotificationUri, theProxyUri);
  myAppContextManager.start();

  AppInfo myAppInfo("name", "provider", "1.0.0", "example app", "");

  // invalid individual router request
  ASSERT_THROW(theProxy.edgeRouter("", "name"), std::runtime_error);
  ASSERT_THROW(theProxy.edgeRouter("1.1.1.1", ""), std::runtime_error);
  ASSERT_THROW(theProxy.edgeRouter("", ""), std::runtime_error);

  // application does not yet exist
  ASSERT_THROW(myAppContextManager.contextCreate(myAppInfo),
               std::runtime_error);
  ASSERT_EQ(0u, theProxy.numContexts());
  ASSERT_TRUE(theProxy.contexts().empty());

  // create the application
  theProxy.addApp(
      AppInfo("name", "provider", "1.0.0", "example app", AppCharcs()));

  // no route to the client
  ASSERT_THROW(myAppContextManager.contextCreate(myAppInfo),
               std::runtime_error);
  ASSERT_EQ(0u, theProxy.numContexts());

  // add a default route for a given app
  theProxy.associateAddress("", "name", "default");

  ASSERT_EQ("default", theProxy.edgeRouter("any-address", "name"));

  // now the context is created with success, with referenceURI == default
  std::map<std::string, AppContextManager*> myAppUeIDs;
  const auto ret = myAppContextManager.contextCreate(myAppInfo);
  ASSERT_EQ("default", ret.second);
  ASSERT_EQ("default", myAppContextManager.referenceUri(ret.first));
  myAppUeIDs.emplace(ret.first, &myAppContextManager);
  ASSERT_EQ(1u, theProxy.numContexts());
  ASSERT_TRUE(context("::1", "name", "default"));

  // change the default router
  theProxy.associateAddress("", "name", "another-default");

  ASSERT_EQ("another-default", theProxy.edgeRouter("any-address", "name"));
  ASSERT_TRUE(context("::1", "name", "another-default"));

  // a notification should arrive to the app context manager
  ASSERT_TRUE(support::waitFor<std::string>(
      [&]() { return myAppContextManager.referenceUri(ret.first); },
      "another-default",
      1));

  // create a few more new contexts from another app context manager
  const std::string myNotificationUri2 = "http://localhost:10002";
  auto              myAnotherAppContextManager =
      std::make_unique<AppContextManager>(myNotificationUri2, theProxyUri);
  myAnotherAppContextManager->start();

  for (auto i = 0; i < 5; i++) {
    const auto ret = myAnotherAppContextManager->contextCreate(myAppInfo);
    ASSERT_EQ("another-default", ret.second);
    ASSERT_EQ("another-default",
              myAnotherAppContextManager->referenceUri(ret.first));
    myAppUeIDs.emplace(ret.first, myAnotherAppContextManager.get());
  }
  ASSERT_EQ(myAppUeIDs.size(), theProxy.numContexts());

  // change router for a non-existing client
  theProxy.associateAddress("::2", "name", "mars");

  // nothing should have changed
  std::this_thread::sleep_for(std::chrono::seconds(1));
  for (const auto& elem : myAppUeIDs) {
    assert(elem.second != nullptr);
    ASSERT_EQ("another-default", elem.second->referenceUri(elem.first));
  }

  // now change the router a the actual clients with application contexts
  theProxy.associateAddress("::1", "name", "moon");

  // the edge router should be update for all
  const auto myCheckAll = [&](const std::string& aExpected) {
    auto myAsExpected = true;
    for (const auto& elem : myAppUeIDs) {
      assert(elem.second != nullptr);
      myAsExpected &= elem.second->referenceUri(elem.first) == aExpected;
    }
    return myAsExpected;
  };
  ASSERT_TRUE(
      support::waitFor<bool>([&]() { return myCheckAll("moon"); }, true, 1));
  ASSERT_EQ(myAppUeIDs.size(), theProxy.numContexts());

  // remove association from the address of all contexts so far
  theProxy.removeAddress("::1", "name");

  // all applications should fall back on the default router
  ASSERT_TRUE(support::waitFor<bool>(
      [&]() { return myCheckAll("another-default"); }, true, 1));
  ASSERT_EQ(myAppUeIDs.size(), theProxy.numContexts());

  // change default router again
  theProxy.associateAddress("", "name", "venus");

  // all applications must update to that
  ASSERT_TRUE(
      support::waitFor<bool>([&]() { return myCheckAll("venus"); }, true, 1));
  ASSERT_EQ(myAppUeIDs.size(), theProxy.numContexts());

  // terminate the second context manager
  myAnotherAppContextManager.reset();

  // deleting the context manager also forces the contexts to be removed
  ASSERT_TRUE(support::waitFor<size_t>(
      [&]() { return theProxy.numContexts(); }, 1, 1.0));
}

} // namespace etsimec
} // namespace uiiit
