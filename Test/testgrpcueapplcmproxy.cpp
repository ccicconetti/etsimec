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

#include "gtest/gtest.h"

#include "EtsiMec/grpcueapplcmproxy.h"
#include "EtsiMec/grpcueapplcmproxyclient.h"

#include <glog/logging.h>

#include <set>

#include "trivialstaticueapplcmproxy.h"

namespace uiiit {
namespace etsimec {

struct TestGrpcUeAppLcmProxy : public ::testing::Test {
  TestGrpcUeAppLcmProxy()
      : theProxyUri("http://localhost:10000")
      , theEndpoint("127.0.0.1:10001") {
  }

  const std::string theProxyUri;
  const std::string theEndpoint;

  static AppInfo makeApp(const std::string& aLambda) {
    return AppInfo(aLambda, "OpenLambdaMec", "1.0", "", AppCharcs());
  }

  using Table = std::list<std::tuple<std::string, std::string, std::string>>;

  bool check(const Table& aLhs, const Table& aRhs) {
    std::set<std::string> myLhs;
    for (const auto& elem : aLhs) {
      myLhs.insert(std::get<0>(elem) + std::get<1>(elem) + std::get<2>(elem));
    }

    std::set<std::string> myRhs;
    for (const auto& elem : aRhs) {
      myRhs.insert(std::get<0>(elem) + std::get<1>(elem) + std::get<2>(elem));
    }

    return myLhs == myRhs;
  }

  static bool contextExists(GrpcUeAppLcmProxyClient& aClient,
                      const std::string&       aAddress,
                      const std::string&       aAppName,
                      const std::string&       aEdgeRouter) {
    for (const auto& elem : aClient.contexts()) {
      if (std::get<0>(elem) == aAddress and std::get<1>(elem) == aAppName and
          std::get<2>(elem) == aEdgeRouter) {
        return true;
      }
    }
    return false;
  }
};

TEST_F(TestGrpcUeAppLcmProxy, test_ctor) {
  StaticUeAppLcmProxy myProxy(theProxyUri);
  ASSERT_NO_THROW(GrpcUeAppLcmProxy(theEndpoint, myProxy));
}

TEST_F(TestGrpcUeAppLcmProxy, test_addresses) {
  StaticUeAppLcmProxy myProxy(theProxyUri);
  GrpcUeAppLcmProxy   myServer(theEndpoint, myProxy);
  myProxy.start();
  myServer.run(false); // non-blocking
  GrpcUeAppLcmProxyClient myClient(theEndpoint);

  // check initial conditions
  ASSERT_EQ(0u, myClient.numContexts());
  ASSERT_EQ((Table()), myClient.table());

  //
  // check default routes
  //

  // add a few routes
  Table myTable1;
  Table myTable2;
  for (auto i = 0; i < 5; i++) {
    myClient.associateAddress(
        "10.0.0." + std::to_string(i), "lambda0", "1.1.1.1");
    myTable1.emplace_back(
        std::make_tuple<std::string, std::string, std::string>(
            "10.0.0." + std::to_string(i), "lambda0", "1.1.1.1"));
    myTable2.emplace_back(
        std::make_tuple<std::string, std::string, std::string>(
            "10.0.0." + std::to_string(i),
            "lambda0",
            (i == 2) ? "1.1.1.2" : "1.1.1.1"));
  }
  ASSERT_TRUE(check(myTable1, myClient.table()));

  // change a route
  myClient.associateAddress("10.0.0.2", "lambda0", "1.1.1.2");
  ASSERT_TRUE(check(myTable2, myClient.table()));

  // associate a client with empty app name: throws
  ASSERT_THROW(myClient.associateAddress("10.0.0.2", "", "1.1.1.1"),
               std::runtime_error);
  // associate a client to an empty address: throws
  ASSERT_THROW(myClient.associateAddress("10.0.0.2", "lambda0", ""),
               std::runtime_error);
  // in all cases, no effect on the proxy
  ASSERT_TRUE(check(myTable2, myClient.table()));

  // create another client, check that they see the same things
  {
    GrpcUeAppLcmProxyClient myAnotherClient(theEndpoint);
    ASSERT_EQ(0u, myAnotherClient.numContexts());
    ASSERT_TRUE(check(myTable2, myClient.table()));
  }

  // remove all the associations
  for (auto i = 0; i < 5; i++) {
    myClient.removeAddress("10.0.0." + std::to_string(i), "lambda0");
  }
  ASSERT_EQ((Table()), myClient.table());

  //
  // check invidual routes
  //

  // unassociate
  ASSERT_TRUE(myProxy.edgeRouter("1.1.1.1", "lambda0").empty());

  // now it is associate
  myClient.associateAddress("1.1.1.1", "lambda0", "edge0");
  ASSERT_EQ("edge0", myProxy.edgeRouter("1.1.1.1", "lambda0"));

  // different address: association does not change
  myClient.associateAddress("1.1.1.2", "lambda0", "edge1");
  ASSERT_EQ("edge0", myProxy.edgeRouter("1.1.1.1", "lambda0"));

  // different lambda: association does not change
  myClient.associateAddress("1.1.1.1", "lambda2", "edge1");
  ASSERT_EQ("edge0", myProxy.edgeRouter("1.1.1.1", "lambda0"));

  // default address: association does not change
  myClient.associateAddress("", "lambda2", "edge1");
  ASSERT_EQ("edge0", myProxy.edgeRouter("1.1.1.1", "lambda0"));

  // individual matching association changed
  myClient.associateAddress("1.1.1.1", "lambda0", "edge1");
  ASSERT_EQ("edge1", myProxy.edgeRouter("1.1.1.1", "lambda0"));

  // remove association
  myClient.removeAddress("1.1.1.1", "lambda0");
  ASSERT_TRUE(myProxy.edgeRouter("1.1.1.1", "lambda0").empty());
}

TEST_F(TestGrpcUeAppLcmProxy, test_lambda) {
  StaticUeAppLcmProxy myProxy(theProxyUri);
  GrpcUeAppLcmProxy   myServer(theEndpoint, myProxy);
  myProxy.start();
  myServer.run(false); // non-blocking
  GrpcUeAppLcmProxyClient myClient(theEndpoint);

  // check two given lambdas do not (yet) exist
  ASSERT_FALSE(myProxy.exists(makeApp("lambda0")));

  // add a lambda from gRPC
  myClient.addLambda("lambda0");
  ASSERT_TRUE(myProxy.exists(makeApp("lambda0")));

  // re-add it
  myClient.addLambda("lambda0");
  ASSERT_TRUE(myProxy.exists(makeApp("lambda0")));

  // remove lambda from gRPC
  myClient.delLambda("lambda0");
  ASSERT_FALSE(myProxy.exists(makeApp("lambda0")));
}

TEST_F(TestGrpcUeAppLcmProxy, test_contexts) {
  TrivialStaticUeAppLcmProxy myProxy(theProxyUri, {"lambda0", "lambda1"});
  GrpcUeAppLcmProxy          myServer(theEndpoint, myProxy);
  myProxy.start();
  myServer.run(false); // non-blocking
  GrpcUeAppLcmProxyClient myClient(theEndpoint);

  // check initial conditions
  ASSERT_EQ(0u, myClient.numContexts());
  ASSERT_TRUE(myClient.contexts().empty());

  // try adding a context with no route
  ASSERT_FALSE(myProxy.add("1.1.1.1", "lambda0"));

  // add a default route for lambda0
  myClient.associateAddress("", "lambda0", "edge0");

  // add an individual route for 1.1.1.1 and lambda1
  myClient.associateAddress("1.1.1.1", "lambda1", "edge1");

  // add a few contexts, with success
  ASSERT_TRUE(myProxy.add("1.1.1.1", "lambda0"));
  ASSERT_TRUE(myProxy.add("1.1.1.1", "lambda0")); // same
  ASSERT_TRUE(myProxy.add("1.1.1.1", "lambda1"));
  ASSERT_TRUE(myProxy.add("1.1.1.2", "lambda0"));

  ASSERT_EQ(4u, myClient.numContexts());
  ASSERT_TRUE(contextExists(myClient, "1.1.1.1", "lambda0", "edge0"));
  ASSERT_TRUE(contextExists(myClient, "1.1.1.1", "lambda0", "edge0"));
  ASSERT_TRUE(contextExists(myClient, "1.1.1.1", "lambda1", "edge1"));
  ASSERT_TRUE(contextExists(myClient, "1.1.1.1", "lambda0", "edge0"));
}

} // namespace etsimec
} // namespace uiiit
