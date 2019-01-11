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

#include "EtsiMec/etsimecoptions.h"

#include <cpprest/base_uri.h>

namespace uiiit {
namespace etsimec {

EtsiMecOptions::EtsiMecOptions(
    int                                          argc,
    char**                                       argv,
    const std::string&                           aDefaultApiRoot,
    const bool                                   aDefaultApiRootRequired,
    boost::program_options::options_description& aDesc)
    : support::CliOptions(argc, argv, aDesc)
    , theApiRoot() {
  // clang-format off
  theDesc.add_options()
  ("etsi-api-root",
   boost::program_options::value<std::string>(&theApiRoot)->default_value(aDefaultApiRoot),
   "ETSI MEC API root.")
  ;
  // clang-format on

  parse();

  if (aDefaultApiRootRequired and (theApiRoot.empty() or not web::uri::validate(theApiRoot))) {
    throw std::runtime_error(
        "The UE app LCM proxy root cannot be empty or invalid");
  }
}

const std::string& EtsiMecOptions::apiRoot() const noexcept {
  return theApiRoot;
}

} // namespace etsimec
} // namespace uiiit
