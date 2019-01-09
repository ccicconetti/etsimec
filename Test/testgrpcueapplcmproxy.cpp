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
};

TEST_F(TestGrpcUeAppLcmProxy, test_ctor) {
  StaticUeAppLcmProxy myProxy(theProxyUri);
  ASSERT_NO_THROW(GrpcUeAppLcmProxy(theEndpoint, myProxy));
}

TEST_F(TestGrpcUeAppLcmProxy, test_client_server) {
  StaticUeAppLcmProxy myProxy(theProxyUri);
  GrpcUeAppLcmProxy   myGrpc(theEndpoint, myProxy);
  myProxy.start();
  myGrpc.run(false); // non-blocking
}

} // namespace etsimec
} // namespace uiiit