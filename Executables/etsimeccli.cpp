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

#include "EtsiMec/appinfo.h"
#include "EtsiMec/applistclient.h"
#include "Rest/server.h"
#include "Support/glograii.h"
#include "Support/split.h"

#include <glog/logging.h>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

namespace po = boost::program_options;

std::string command(const std::string& aApiRoot,
                    const std::string& aCommand,
                    const bool         aVerbose,
                    const bool         aMattermost) {
  std::stringstream myStream;

  if (aApiRoot.empty()) {
    throw std::runtime_error("Empty API root");
  }
  if (not web::uri::validate(aApiRoot)) {
    throw std::runtime_error("Invalid API root: " + aApiRoot);
  }

  if (aCommand == "app_list") {

    if (aMattermost) {
      myStream << " | appName  | appProvider | appSoftVersion |\n"
                  " | -------- | ----------- | -------------- |\n";
    }
    for (const auto& myAppInfo : uiiit::etsimec::AppListClient(aApiRoot)()) {
      if (aMattermost) {
        myStream << " | " << myAppInfo.appName() << " | "
                 << myAppInfo.appProvider() << " | "
                 << myAppInfo.appSoftVersion() << " | " << std::endl;
      } else {
        if (aVerbose) {
          myStream << myAppInfo << std::endl;
        } else {
          myStream << myAppInfo.appName() << ", " << myAppInfo.appProvider()
                   << ", " << myAppInfo.appSoftVersion() << std::endl;
        }
      }
    }

  } else {
    throw std::runtime_error("Unknown command: " + aCommand);
  }

  return myStream.str();
}

class MattermostProxy : public uiiit::rest::Server
{
 public:
  MattermostProxy(const std::string& aUri, const std::string& aToken)
      : uiiit::rest::Server(aUri)
      , theToken(aToken) {
    (*this)(web::http::methods::POST,
            "(.*)",
            [this](web::http::http_request aReq) { handlePost(aReq); });
  }

  void handlePost(web::http::http_request aReq) {
    std::string myBody;
    aReq.extract_string()
        .then([&myBody](pplx::task<utility::string_t> aPrevTask) {
          myBody = aPrevTask.get();
        })
        .wait();

    const auto               myCommands   = web::uri::split_query(myBody);
    auto                     myAuthorized = false;
    std::string              myUsername;
    std::vector<std::string> myText;
    for (const auto& myPair : myCommands) {
      const auto myDecoded = web::uri::decode(myPair.second);
      LOG(INFO) << myPair.first << " : " << myDecoded;
      if (myPair.first == "token") {
        if (myDecoded == theToken) {
          myAuthorized = true;
        }
      } else if (myPair.first == "user_name") {
        myUsername = myDecoded;
      } else if (myPair.first == "text") {
        myText =
            uiiit::support::split<std::vector<std::string>>(myDecoded, "+");
      }
    }

    web::json::value myRet;
    if (not myAuthorized) {
      aReq.reply(web::http::status_codes::Unauthorized, myRet);
    }

    auto        myFail    = false;
    auto        it        = myText.begin();
    std::string myCommand = "app_list";
    std::string myApiRoot;
    for (; it != myText.end(); ++it) {
      if (*it == "command") {
        ++it;
        if (it == myText.end()) {
          myFail = true;
          break;
        } else {
          myCommand = *it;
        }
      } else if (*it == "apiroot") {
        ++it;
        if (it == myText.end()) {
          myFail = true;
          break;
        } else {
          myApiRoot = *it;
        }
      }
    }

    if (myApiRoot.empty() or myFail) {
      std::string myInput("Invalid input:");
      for (const auto& myValue : myText) {
        myInput.append(" ");
        myInput.append(myValue);
      }
      myRet["text"] = web::json::value(myInput);
    } else {
      try {
        myRet["text"] = web::json::value(
            "Hello " + myUsername + ", the list of ETSI MEC applications on " +
            myApiRoot + " is:\n\n" +
            command(myApiRoot, myCommand, false, true));
      } catch (const std::runtime_error& aErr) {
        myRet["text"] = web::json::value(
            std::string("Invalid response from ETSI MEC server: ") +
            aErr.what());
      } catch (...) {
        myRet["text"] =
            web::json::value("Invalid response from ETSI MEC server");
      }
    }

    aReq.reply(web::http::status_codes::OK, myRet);
  }

 private:
  const std::string theToken;
};

int main(int argc, char* argv[]) {
  uiiit::support::GlogRaii myGlogRaii(argv[0]);

  std::string             myApiRoot;
  std::string             myMmProxyRoot;
  std::string             myMmToken;
  std::string             myCommand;
  po::options_description myDesc("Allowed options");

  // clang-format off
  myDesc.add_options()
  ("help,h", "produce help message")
  ("api-root",
   po::value<std::string>(&myApiRoot)->default_value("http://localhost:6500"),
   "API root of the ETSI MEC server.")
  ("command",
   po::value<std::string>(&myCommand)->default_value("app_list"),
   "One of: {app_list}.")
  ("mattermost-proxy",
   po::value<std::string>(&myMmProxyRoot)->default_value(""),
   "Act as a proxy for Mattermost using the given root URI.")
  ("mattermost-token",
   po::value<std::string>(&myMmToken)->default_value(""),
   "Use this token to validate Mattermost requests.")
  ("mattermost-output", "Pretty format the output for Mattermost.")
  ("verbose", "Be verbose.")
  ;
  // clang-format on

  try {
    po::variables_map myVarMap;
    po::store(po::parse_command_line(argc, argv, myDesc), myVarMap);
    po::notify(myVarMap);

    if (myVarMap.count("help")) {
      std::cout << myDesc << std::endl;
      return EXIT_FAILURE;
    }

    const auto myVerbose    = myVarMap.count("verbose") > 0;
    const auto myMattermost = myVarMap.count("mattermost-output") > 0;

    if (myVerbose and myMattermost) {
      throw std::runtime_error(
          "Incompatible options --verbose and --mattermost-output");
    }

    if (not myMmProxyRoot.empty()) {
      if (not web::uri::validate(myMmProxyRoot)) {
        throw std::runtime_error("Invalid Mattermost root: " + myMmProxyRoot);
      }
      MattermostProxy myMmProxy(myMmProxyRoot, myMmToken);
      myMmProxy.start();
      pause();

    } else {
      std::cout << command(myApiRoot, myCommand, myVerbose, myMattermost);
    }

    return EXIT_SUCCESS;
  } catch (const std::exception& aErr) {
    LOG(ERROR) << "Exception caught: " << aErr.what();
  } catch (...) {
    LOG(ERROR) << "Unknown exception caught";
  }

  return EXIT_FAILURE;
}
