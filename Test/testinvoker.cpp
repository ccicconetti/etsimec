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

#include "OpenWhisk/invoker.h"

#include "wskutils.h"

#include "gtest/gtest.h"

#include <glog/logging.h>

#include <cstdlib>

namespace uiiit {
namespace wsk {

struct TestInvoker : public ::testing::Test {};

TEST_F(TestInvoker, test_ctor) {
  ASSERT_THROW(Invoker("", "X"), std::runtime_error);
  ASSERT_THROW(Invoker("X", ""), std::runtime_error);
  ASSERT_NO_THROW(Invoker("X", "X"));
}

TEST_F(TestInvoker, test_invoke_unreachable) {
  Invoker myInvoker("https://127.0.0.1:4", "invalid-token");

  const auto res = myInvoker("hello", "");

  ASSERT_FALSE(res.first);
  LOG(INFO) << res.second;
}

TEST_F(TestInvoker, test_invoke_map) {
  SKIP_IF_WSK_NOT_READY

  Invoker myInvoker(WskEnv::apiHost(), WskEnv::auth());

  auto res = myInvoker(
      "uiiitrest-test-invoke", "", {{"name", "Mickey"}, {"surname", "Mouse"}});

  ASSERT_TRUE(res.first);
  ASSERT_EQ("Hello, Mr./Ms. Mickey Mouse!", res.second);

  res = myInvoker("uiiitrest-test-invoke",
                  "guest",
                  {{"name", "Mickey"}, {"surname", "Mouse"}});

  ASSERT_TRUE(res.first);
  ASSERT_EQ("Hello, Mr./Ms. Mickey Mouse!", res.second);

  res = myInvoker("uiiitrest-test-invoke",
                  "non-existing-namespace",
                  {{"name", "Mickey"}, {"surname", "Mouse"}});

  ASSERT_FALSE(res.first);
  LOG(INFO) << res.second;
}

TEST_F(TestInvoker, test_invoke_json) {
  SKIP_IF_WSK_NOT_READY

  Invoker myInvoker(WskEnv::apiHost(), WskEnv::auth());

  const auto res = myInvoker("uiiitrest-test-invoke",
                             "",
                             "{\"name\": \"Mickey\", \"surname\": \"Mouse\"}");

  ASSERT_TRUE(res.first);
  ASSERT_EQ("Hello, Mr./Ms. Mickey Mouse!", res.second);
}

TEST_F(TestInvoker, test_invoke_no_params) {
  SKIP_IF_WSK_NOT_READY

  Invoker myInvoker(WskEnv::apiHost(), WskEnv::auth());

  const auto res = myInvoker("uiiitrest-test-invoke", "");

  ASSERT_TRUE(res.first);
  ASSERT_EQ("Hello, Mr./Ms. <name unknown> <surname unknown>!", res.second);
}

} // namespace wsk
} // namespace uiiit
