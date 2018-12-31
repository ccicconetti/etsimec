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

struct TestEtsiMec : public ::testing::Test {
  TestEtsiMec()
      : theProxyUri("http://localhost:10000"),
        theInputFile("to_remove/staticfile.txt") {
    boost::filesystem::remove_all("to_remove");
    boost::filesystem::create_directories("to_remove");
  }

  ~TestEtsiMec() { boost::filesystem::remove_all("to_remove"); }

  void countEq(rest::Client &aClient, const size_t aExpected) {
    const auto myRet = aClient.get("/app_list");
    ASSERT_EQ(web::http::status_codes::OK, myRet.first);
    ASSERT_TRUE(myRet.second.has_object_field("ApplicationList"));
    ASSERT_TRUE(myRet.second.at("ApplicationList").has_array_field("appInfo"));
    ASSERT_EQ(
        aExpected,
        myRet.second.at("ApplicationList").at("appInfo").as_array().size());
  }

  const std::string theProxyUri;
  const std::string theInputFile;
};

TEST_F(TestEtsiMec, test_appcontext) {
  const AppInfo myInfoRequest("my-app", "my-provider", "v1.0", "",
                              "package-source");
  const AppInfo myInfoResponse("my-app", "my-provider", "v1.0", "", "ref-uri",
                               "package-source");
  const AppInfo myInfoWrong("my-app", "my-provider", "v1.0", "", AppCharcs());

  // check type
  ASSERT_EQ(AppContext::Type::Request,
            AppContext("ue-app-id", "callback-ref", myInfoRequest).type());
  ASSERT_EQ(
      AppContext::Type::Response,
      AppContext("context-id", "ue-app-id", "callback-ref", myInfoResponse)
          .type());

  // valid
  ASSERT_NO_THROW(AppContext("ue-app-id", "callback-ref", myInfoRequest));
  ASSERT_NO_THROW(
      AppContext("context-id", "ue-app-id", "callback-ref", myInfoResponse));
  ASSERT_NO_THROW(AppContext("ue-app-id", "", myInfoRequest));
  ASSERT_NO_THROW(AppContext("context-id", "ue-app-id", "", myInfoResponse));
  ASSERT_NO_THROW(AppContext("ue-app-id", "callback-ref", myInfoRequest)
                      .makeResponse("context-id", "ref-uri"));

  // invalid
  ASSERT_THROW(
      AppContext("context-id", "ue-app-id", "callback-ref", myInfoRequest),
      std::runtime_error);
  ASSERT_THROW(AppContext("ue-app-id", "callback-ref", myInfoResponse),
               std::runtime_error);
  ASSERT_THROW(AppContext("ue-app-id", "callback-ref", myInfoWrong),
               std::runtime_error);
  ASSERT_THROW(
      AppContext("context-id", "ue-app-id", "callback-ref", myInfoWrong),
      std::runtime_error);

  ASSERT_THROW(AppContext("", "callback-ref", myInfoRequest),
               std::runtime_error);

  ASSERT_THROW(AppContext("", "ue-app-id", "callback-ref", myInfoRequest),
               std::runtime_error);
  ASSERT_THROW(AppContext("context-id", "", "callback-ref", myInfoRequest),
               std::runtime_error);

  ASSERT_THROW(AppContext("ue-app-id", "callback-ref", myInfoRequest)
                   .makeResponse("", "ref-uri"),
               std::runtime_error);
  ASSERT_THROW(AppContext("ue-app-id", "callback-ref", myInfoRequest)
                   .makeResponse("context-id", ""),
               std::runtime_error);
  ASSERT_THROW(AppContext("ue-app-id", "callback-ref", myInfoResponse)
                   .makeResponse("context-id", "ref-uri"),
               std::runtime_error);
}

TEST_F(TestEtsiMec, test_ueapplcmproxy_applist) {
  TrivialUeAppLcmProxy myProxy(theProxyUri);
  myProxy.start();

  rest::Client myClient(theProxyUri + "/" + myProxy.apiName() + "/" +
                        myProxy.apiVersion());
  countEq(myClient, 0);

  myProxy.addApp(AppInfo("my-app", "my-provider", "v1.0", "", AppCharcs()));
  countEq(myClient, 1);

  myProxy.addApp(AppInfo("my-app", "my-provider", "v1.0", "", AppCharcs()));
  countEq(myClient, 1);

  myProxy.addApp(AppInfo("my-app2", "my-provider", "v1.0", "", AppCharcs()));
  countEq(myClient, 2);

  myProxy.delApp(AppInfo("my-app2", "my-provider", "v1.0", "", AppCharcs()));
  countEq(myClient, 1);

  myProxy.delApp(AppInfo("my-app2", "my-provider", "v1.0", "", AppCharcs()));
  countEq(myClient, 1);

  myProxy.delApp(AppInfo("my-app3", "my-provider", "v1.0", "", AppCharcs()));
  countEq(myClient, 1);

  myProxy.delApp(AppInfo("my-app", "my-provider", "v1.0", "", AppCharcs()));
  countEq(myClient, 0);
}

TEST_F(TestEtsiMec, test_ueapplcmproxy_appcontext) {
  TrivialUeAppLcmProxy myProxy(theProxyUri);
  myProxy.start();
  ASSERT_TRUE(myProxy.theClientAddress.empty());

  rest::Client myClient(theProxyUri + "/" + myProxy.apiName() + "/" +
                        myProxy.apiVersion());

  // create a new context
  AppContext myContext("unique-id", "callback-ref",
                       AppInfo("my-app", "my-provider", "v1.0", "", "package"));
  const auto ret = myClient.post(myContext.toJson(), "/app_contexts");
  ASSERT_EQ(web::http::status_codes::Created, ret.first);
  ASSERT_EQ("context-id-1", AppContext(ret.second).contextId());
  ASSERT_FALSE(myProxy.theClientAddress.empty());

  // update it
  ASSERT_EQ(web::http::status_codes::NoContent,
            myClient.put(myContext.toJson(), "/app_contexts/xxx").first);
  ASSERT_EQ(1u, myProxy.theUpdated);

  // delete it
  ASSERT_EQ(web::http::status_codes::NoContent,
            myClient.del("/app_contexts/xxx"));
  ASSERT_EQ("xxx", myProxy.theContextId);

  //
  // do the same as above, but now the requests are marked as invalid
  //
  myProxy.theValidRequest = false;

  // create a new context
  ASSERT_EQ(web::http::status_codes::Forbidden,
            myClient.post(myContext.toJson(), "/app_contexts").first);

  // update it
  ASSERT_EQ(web::http::status_codes::Forbidden,
            myClient.put(myContext.toJson(), "/app_contexts/xxx").first);
  ASSERT_EQ(2u, myProxy.theUpdated);

  // delete it
  ASSERT_EQ(web::http::status_codes::Forbidden,
            myClient.del("/app_contexts/xxx"));
  ASSERT_EQ("xxx", myProxy.theContextId);
}

TEST_F(TestEtsiMec, test_appcontextmanager) {
  const std::string myNotificationUri = "http://localhost:10001";
  TrivialUeAppLcmProxy myProxy(theProxyUri);
  myProxy.start();
  AppContextManager myAppContextManager(myNotificationUri, theProxyUri);
  myAppContextManager.start();

  // create some contexts
  for (auto i = 1; i <= 5; i++) {
    AppInfo myAppInfo("name", "provider", "1.0.0", "example app", "");
    const auto myRefUri = myAppContextManager.contextCreate(myAppInfo).second;
    ASSERT_EQ("reference-uri-" + std::to_string(i), myRefUri);
  }

  // ill-formed request, must throw
  ASSERT_THROW(myAppContextManager.contextCreate(AppInfo(
                   "name", "provider", "1.0.0", "example app", "ref-uri", "")),
               std::runtime_error);

  // server-side refusal, must throw
  myProxy.theValidRequest = false;
  ASSERT_THROW(myAppContextManager.contextCreate(
                   AppInfo("name", "provider", "1.0.0", "example app", "")),
               std::runtime_error);
}

TEST_F(TestEtsiMec, test_appcontextmanager_staticueapplcmproxy) {
  const std::string myNotificationUri = "http://localhost:10001";
  StaticUeAppLcmProxy myProxy(theProxyUri);
  myProxy.start();
  AppContextManager myAppContextManager(myNotificationUri, theProxyUri);
  myAppContextManager.start();

  AppInfo myAppInfo("name", "provider", "1.0.0", "example app", "");

  // application does not yet exist
  ASSERT_THROW(myAppContextManager.contextCreate(myAppInfo),
               std::runtime_error);
  ASSERT_EQ(0u, myProxy.numContexts());

  // create the application
  myProxy.addApp(
      AppInfo("name", "provider", "1.0.0", "example app", AppCharcs()));

  // no route to the client
  ASSERT_THROW(myAppContextManager.contextCreate(myAppInfo),
               std::runtime_error);
  ASSERT_EQ(0u, myProxy.numContexts());

  // add a default route
  myProxy.defaultEdgeRouter("default");

  // now the context is created with success, with referenceURI == default
  std::map<std::string, AppContextManager *> myAppUeIDs;
  const auto ret = myAppContextManager.contextCreate(myAppInfo);
  ASSERT_EQ("default", ret.second);
  ASSERT_EQ("default", myAppContextManager.referenceUri(ret.first));
  myAppUeIDs.emplace(ret.first, &myAppContextManager);
  ASSERT_EQ(1u, myProxy.numContexts());

  // change the default router
  myProxy.defaultEdgeRouter("another-default");

  // a notification should arrive to the app context manager
  ASSERT_TRUE(support::waitFor<std::string>(
      [&]() { return myAppContextManager.referenceUri(ret.first); },
      "another-default", 1));

  // create a few more new contexts from another app context manager
  const std::string myNotificationUri2 = "http://localhost:10002";
  auto myAnotherAppContextManager =
      std::make_unique<AppContextManager>(myNotificationUri2, theProxyUri);
  myAnotherAppContextManager->start();

  for (auto i = 0; i < 5; i++) {
    const auto ret = myAnotherAppContextManager->contextCreate(myAppInfo);
    ASSERT_EQ("another-default", ret.second);
    ASSERT_EQ("another-default",
              myAnotherAppContextManager->referenceUri(ret.first));
    myAppUeIDs.emplace(ret.first, myAnotherAppContextManager.get());
  }
  ASSERT_EQ(myAppUeIDs.size(), myProxy.numContexts());

  // change router for a non-existing client
  myProxy.associateAddress("::2", "mars");

  // nothing should have changed
  std::this_thread::sleep_for(std::chrono::seconds(1));
  for (const auto &elem : myAppUeIDs) {
    assert(elem.second != nullptr);
    ASSERT_EQ("another-default", elem.second->referenceUri(elem.first));
  }

  // now change the router a the actual clients with application contexts
  myProxy.associateAddress("::1", "moon");

  // the edge router should be update for all
  const auto myCheckAll = [&](const std::string &aExpected) {
    auto myAsExpected = true;
    for (const auto &elem : myAppUeIDs) {
      assert(elem.second != nullptr);
      myAsExpected &= elem.second->referenceUri(elem.first) == aExpected;
    }
    return myAsExpected;
  };
  ASSERT_TRUE(
      support::waitFor<bool>([&]() { return myCheckAll("moon"); }, true, 1));
  ASSERT_EQ(myAppUeIDs.size(), myProxy.numContexts());

  // remove association from the address of all contexts so far
  myProxy.removeAddress("::1");

  // all applications should fall back on the default router
  ASSERT_TRUE(support::waitFor<bool>(
      [&]() { return myCheckAll("another-default"); }, true, 1));
  ASSERT_EQ(myAppUeIDs.size(), myProxy.numContexts());

  // change default router again
  myProxy.defaultEdgeRouter("venus");

  // all applications must update to that
  ASSERT_TRUE(
      support::waitFor<bool>([&]() { return myCheckAll("venus"); }, true, 1));
  ASSERT_EQ(myAppUeIDs.size(), myProxy.numContexts());

  // terminate the second context manager
  myAnotherAppContextManager.reset();

  // deleting the context manager also forces the contexts to be removed
  ASSERT_TRUE(support::waitFor<size_t>([&]() { return myProxy.numContexts(); },
                                       1, 1.0));
}

TEST_F(TestEtsiMec, test_staticfileueapplcmproxy) {
  // file does not exist
  ASSERT_THROW(StaticFileUeAppLcmProxy(theProxyUri, theInputFile),
               std::runtime_error);

  // file is empty
  { std::ofstream myFile(theInputFile); }
  ASSERT_THROW(StaticFileUeAppLcmProxy(theProxyUri, theInputFile),
               std::runtime_error);

  // file is ill-formed
  {
    std::ofstream myFile(theInputFile);
    myFile << "a b c";
  }
  ASSERT_THROW(StaticFileUeAppLcmProxy(theProxyUri, theInputFile),
               std::runtime_error);

  // valid file
  {
    std::ofstream myFile(theInputFile);
    myFile << "earth planet\nmars planet\npluto non-planet\n";
  }
  StaticFileUeAppLcmProxy myProxy(theProxyUri, theInputFile);
  ASSERT_EQ(3u, myProxy.numAddresses());
}

} // namespace etsimec
} // namespace uiiit
