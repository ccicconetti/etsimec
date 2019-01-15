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

#include "EtsiMec/grpcueapplcmproxy.h"
#include "EtsiMec/grpcueapplcmproxyclient.h"
#include "EtsiMec/staticueapplcmproxy.h"

#include <glog/logging.h>

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
};

TEST_F(TestGrpcUeAppLcmProxy, test_ctor) {
  StaticUeAppLcmProxy myProxy(theProxyUri);
  ASSERT_NO_THROW(GrpcUeAppLcmProxy(theEndpoint, myProxy));
}

TEST_F(TestGrpcUeAppLcmProxy, test_client_server) {
  StaticUeAppLcmProxy myProxy(theProxyUri);
  GrpcUeAppLcmProxy   myServer(theEndpoint, myProxy);
  myProxy.start();
  myServer.run(false); // non-blocking
  GrpcUeAppLcmProxyClient myClient(theEndpoint);

  // check initial conditions
  ASSERT_EQ(0u, myClient.numContexts());
  ASSERT_EQ((Table()), myClient.table());

  // add a few routes
  for (auto i = 0; i < 5; i++) {
    myClient.associateAddress(
        "10.0.0." + std::to_string(i), "lambda0", "1.1.1.1");
  }
  ASSERT_EQ((Table({
                {"10.0.0.4", "lambda0", "1.1.1.1"},
                {"10.0.0.3", "lambda0", "1.1.1.1"},
                {"10.0.0.2", "lambda0", "1.1.1.1"},
                {"10.0.0.1", "lambda0", "1.1.1.1"},
                {"10.0.0.0", "lambda0", "1.1.1.1"},
            })),
            myClient.table());

  // change a route
  myClient.associateAddress("10.0.0.2", "lambda0", "1.1.1.2");
  ASSERT_EQ((Table({
                {"10.0.0.4", "lambda0", "1.1.1.1"},
                {"10.0.0.3", "lambda0", "1.1.1.1"},
                {"10.0.0.2", "lambda0", "1.1.1.2"},
                {"10.0.0.1", "lambda0", "1.1.1.1"},
                {"10.0.0.0", "lambda0", "1.1.1.1"},
            })),
            myClient.table());

  // associate a client with empty app name: throws
  ASSERT_THROW(myClient.associateAddress("10.0.0.2", "", "1.1.1.1"),
               std::runtime_error);
  // associate a client to an empty address: throws
  ASSERT_THROW(myClient.associateAddress("10.0.0.2", "lambda0", ""),
               std::runtime_error);
  // in all cases, no effect on the proxy
  ASSERT_EQ((Table({
                {"10.0.0.4", "lambda0", "1.1.1.1"},
                {"10.0.0.3", "lambda0", "1.1.1.1"},
                {"10.0.0.2", "lambda0", "1.1.1.2"},
                {"10.0.0.1", "lambda0", "1.1.1.1"},
                {"10.0.0.0", "lambda0", "1.1.1.1"},
            })),
            myClient.table());

  // create another client, check that they see the same things
  {
    GrpcUeAppLcmProxyClient myAnotherClient(theEndpoint);
    ASSERT_EQ(0u, myAnotherClient.numContexts());
    ASSERT_EQ((Table({
                  {"10.0.0.4", "lambda0", "1.1.1.1"},
                  {"10.0.0.3", "lambda0", "1.1.1.1"},
                  {"10.0.0.2", "lambda0", "1.1.1.2"},
                  {"10.0.0.1", "lambda0", "1.1.1.1"},
                  {"10.0.0.0", "lambda0", "1.1.1.1"},
              })),
              myAnotherClient.table());
  }

  // remove all the associations
  for (auto i = 0; i < 5; i++) {
    myClient.removeAddress("10.0.0." + std::to_string(i), "lambda0");
  }
  ASSERT_EQ((Table()), myClient.table());

  //
  // lambda testing now
  //

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

} // namespace etsimec
} // namespace uiiit
