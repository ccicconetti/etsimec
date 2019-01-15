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

#include "EtsiMec/grpcueapplcmproxyclient.h"
#include "Support/clioptions.h"
#include "Support/glograii.h"
#include "Support/split.h"

#include <glog/logging.h>

#include <boost/program_options.hpp>

#include <string>
#include <vector>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  uiiit::support::GlogRaii myGlogRaii(argv[0]);

  po::options_description myDesc(
      "Command-line utility to query a static UE application LCM proxy.\n"
      "\n"
      "Commands are read sequentially from standard input until closed. "
      "Available commands:\n"
      "- associate C A S\n"
      "  associates the address of the edge client C (* means \"any\") and the "
      "application name A to the end-point of the edge router S\n"
      "- remove C A\n"
      "  removes the association of edge client C (* means \"any\") and "
      "application name A, if present\n"
      "- add-lambda X\n"
      "  adds a lambda function to the list of applications\n"
      "- del-lambda X\n"
      "  removes from the list of applications the given lambda\n"
      "- num-contexts\n"
      "  returns the number of active UE application contexts\n"
      "- table\n"
      "  return a space-separate table of the address associations, if "
      "non-empty\n"
      "- quit\n"
      "  exits from the application\n"
      "\n"
      "Every command is given a response on standard output.\n"
      "All the set commands return the string OK if the command was accepted.\n"
      "the string Invalid command if the command is not well-formed.\n\n"
      "Allowed options");
  std::string myServerEndpoint;

  // clang-format off
  myDesc.add_options()
  ("server-endpoint",
   po::value<std::string>(&myServerEndpoint)->default_value("0.0.0.0:6477"),
   "Endpoint of the gRPC interface of this application")
  ;
  // clang-format on

  try {
    uiiit::support::TrivialOptions myCli(argc, argv, myDesc);

    uiiit::etsimec::GrpcUeAppLcmProxyClient myClient(myServerEndpoint);

    std::string myLine;
    while (std::cin) {
      std::getline(std::cin, myLine);
      const auto myTokens =
          uiiit::support::split<std::vector<std::string>>(myLine, " ");
      if (myTokens.empty()) {
        continue;
      }

      const auto& myCommand = myTokens[0];

      if (myCommand == "associate" and myTokens.size() == 4) {
        const auto myAddress = myTokens[1] == "*" ? std::string() : myTokens[1];
        myClient.associateAddress(myAddress, myTokens[2], myTokens[3]);
        std::cout << "OK" << std::endl;

      } else if (myCommand == "remove" and myTokens.size() == 3) {
        const auto myAddress = myTokens[1] == "*" ? std::string() : myTokens[1];
        myClient.removeAddress(myAddress, myTokens[2]);
        std::cout << "OK" << std::endl;

      } else if (myCommand == "add-lambda" and myTokens.size() == 2) {
        myClient.addLambda(myTokens[1]);
        std::cout << "OK" << std::endl;

      } else if (myCommand == "del-lambda" and myTokens.size() == 2) {
        myClient.delLambda(myTokens[1]);
        std::cout << "OK" << std::endl;

      } else if (myCommand == "num-contexts" and myTokens.size() == 1) {
        std::cout << myClient.numContexts() << std::endl;

      } else if (myCommand == "table" and myTokens.size() == 1) {
        for (const auto& elem : myClient.table()) {
          std::cout << (std::get<0>(elem).empty() ? "*" : std::get<0>(elem))
                    << ' ' << std::get<1>(elem) << ' ' << std::get<2>(elem)
                    << '\n';
        }
        std::cout << std::flush;

      } else if (myCommand == "quit" and myTokens.size() == 1) {
        break;

      } else {
        std::cout << "Invalid command" << std::endl;
      }
    }

    return EXIT_SUCCESS;
  } catch (const uiiit::support::CliExit&) {
    return EXIT_SUCCESS; // clean exit
  } catch (const std::exception& aErr) {
    LOG(ERROR) << "Exception caught: " << aErr.what();
  } catch (...) {
    LOG(ERROR) << "Unknown exception caught";
  }

  return EXIT_FAILURE;
}
