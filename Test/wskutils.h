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

#include <glog/logging.h>

#include <cstdlib>

namespace uiiit {
namespace wsk {

/*
 *
 * It is assumed that a function called 'hello' exists
 * on the given OpenWhisk server, whose URL and authorization
 * token are given in environment variables UIIITREST_APIHOST
 * and UIIITREST_AUTH

function main (params) {
  var name = params.name || '<name unknown>'
  var surname = params.surname || '<surname unknown>'
  return { payload: 'Hello, Mr./Ms. ' + name + ' ' + surname + '!' }
}

 * Without the environment variables set, the test is not executed.
 */
class WskEnv
{
 public:
  static bool ready() {
    if (getenv("UIIITREST_APIHOST") == nullptr or
        getenv("UIIITREST_AUTH") == nullptr) {
      LOG(INFO) << "test not executed, must configure UIIITREST_APIHOST and "
                   "UIIITREST_AUTH";
      return false;
    }
    return true;
  }

  static std::string apiHost() {
    return getenv("UIIITREST_APIHOST");
  }
  static std::string auth() {
    return getenv("UIIITREST_AUTH");
  }
};

#define SKIP_IF_WSK_NOT_READY                                                  \
  if (not WskEnv::ready()) {                                                   \
    return;                                                                    \
  }

} // namespace wsk
} // namespace uiiit
