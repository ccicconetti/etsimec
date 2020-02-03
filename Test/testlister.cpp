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

#include "OpenWhisk/lister.h"

#include "wskutils.h"

#include "gtest/gtest.h"

#include <cpprest/http_msg.h>

#include <glog/logging.h>

#include <cstdlib>

namespace uiiit {
namespace wsk {

struct TestLister : public ::testing::Test {};

TEST_F(TestLister, test_ctor) {
  ASSERT_THROW(Lister("", "X"), std::runtime_error);
  ASSERT_THROW(Lister("X", ""), std::runtime_error);
  ASSERT_NO_THROW(Lister("X", "X"));
}

TEST_F(TestLister, test_invoke_unreachable) {
  Lister myLister("https://127.0.0.1:4", "invalid-token");

  ASSERT_THROW(myLister(0, 0), web::http::http_exception);
}

TEST_F(TestLister, test_invoke) {
  SKIP_IF_WSK_NOT_READY

  Lister myLister(WskEnv::apiHost(), WskEnv::auth());

  auto   myFound = false;
  size_t mySkip  = 0;
  for (/* no init */; /* no cond */; mySkip++) {
    const auto myList = myLister(1, mySkip);
    ASSERT_GE(1u, myList.size());
    if (myList.empty()) {
      break;
    }
    if (myList.begin()->first.theName == "uiiitrest-test-invoke") {
      LOG(INFO) << "found " << myList.begin()->second.toString();
      myFound = true;
    }
  }

  ASSERT_TRUE(myFound);
  LOG(INFO) << "total actions found at " << myLister.apiRoot() << ": " << mySkip;
}

} // namespace wsk
} // namespace uiiit
