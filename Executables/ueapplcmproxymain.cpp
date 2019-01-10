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
#include "EtsiMec/grpcueapplcmproxy.h"
#include "EtsiMec/staticueapplcmproxy.h"
#include "Support/glograii.h"

#include <glog/logging.h>

#include <boost/program_options.hpp>

#include <cstdlib>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  uiiit::support::GlogRaii myGlogRaii(argv[0]);

  po::options_description myDesc("Allowed options");
  std::string             myServerEndpoint;

  // clang-format off
  myDesc.add_options()
  ("server-endpoint",
   po::value<std::string>(&myServerEndpoint)->default_value("0.0.0.0:6477"),
   "Endpoint of the gRPC interface of this application")
  ;
  // clang-format on

  try {
    uiiit::etsimec::EtsiMecOptions myCli(
        argc, argv, "http://127.0.0.1:6500", myDesc);

    if (myCli.apiRoot().empty()) {
      throw std::runtime_error("The UE app LCM proxy root cannot be empty");
    }
    if (myServerEndpoint.empty()) {
      throw std::runtime_error("The gRPC end-point cannot be empty");
    }

    uiiit::etsimec::StaticUeAppLcmProxy myProxy(myCli.apiRoot());
    uiiit::etsimec::GrpcUeAppLcmProxy   myServer(myServerEndpoint, myProxy);
    myProxy.start();
    myServer.run(true); // blocking

    return EXIT_SUCCESS;
  } catch (const std::exception& aErr) {
    LOG(ERROR) << "Exception caught: " << aErr.what();
  } catch (...) {
    LOG(ERROR) << "Unknown exception caught";
  }

  return EXIT_FAILURE;
}
