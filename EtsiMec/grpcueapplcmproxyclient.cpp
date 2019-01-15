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
#include "RpcSupport/utils.h"

namespace uiiit {
namespace etsimec {

GrpcUeAppLcmProxyClient::GrpcUeAppLcmProxyClient(
    const std::string& aServerEndpoint)
    : ::uiiit::rpc::SimpleClient<rpc::UeAppLcmProxy>(aServerEndpoint) {
}

void GrpcUeAppLcmProxyClient::associateAddress(const std::string& aClient,
                                               const std::string& aAppName,
                                               const std::string& aServer) {

  if (aAppName.empty()) {
    throw std::runtime_error("Invalid empty application name");
  }
  if (aServer.empty()) {
    throw std::runtime_error("Invalid empty edge router address");
  }

  grpc::ClientContext myContext;
  rpc::AddressTuple   myReq;
  myReq.set_client(aClient);
  myReq.set_appname(aAppName);
  myReq.set_server(aServer);
  rpc::Void myRep;

  ::uiiit::rpc::checkStatus(
      theStub->associateAddress(&myContext, myReq, &myRep));
}

void GrpcUeAppLcmProxyClient::removeAddress(const std::string& aClient,
                                            const std::string& aAppName) {
  if (aAppName.empty()) {
    throw std::runtime_error("Invalid empty application name");
  }

  grpc::ClientContext myContext;
  rpc::AddressTuple   myReq;
  myReq.set_client(aClient);
  myReq.set_appname(aAppName);
  rpc::Void myRep;

  ::uiiit::rpc::checkStatus(theStub->removeAddress(&myContext, myReq, &myRep));
}

void GrpcUeAppLcmProxyClient::addLambda(const std::string& aLambda) {
  if (aLambda.empty()) {
    throw std::runtime_error("Invalid empty lambda function name");
  }

  grpc::ClientContext myContext;
  rpc::Lambda         myReq;
  myReq.set_value(aLambda);
  rpc::Void myRep;

  ::uiiit::rpc::checkStatus(theStub->addLambda(&myContext, myReq, &myRep));
}

void GrpcUeAppLcmProxyClient::delLambda(const std::string& aLambda) {
  if (aLambda.empty()) {
    throw std::runtime_error("Invalid empty lambda function name");
  }

  grpc::ClientContext myContext;
  rpc::Lambda         myReq;
  myReq.set_value(aLambda);
  rpc::Void myRep;

  ::uiiit::rpc::checkStatus(theStub->delLambda(&myContext, myReq, &myRep));
}

size_t GrpcUeAppLcmProxyClient::numContexts() {
  grpc::ClientContext myContext;
  rpc::Void           myReq;
  rpc::Number         myRep;

  ::uiiit::rpc::checkStatus(theStub->numContexts(&myContext, myReq, &myRep));
  return myRep.value();
}

std::list<std::tuple<std::string, std::string, std::string>>
GrpcUeAppLcmProxyClient::table() {
  grpc::ClientContext myContext;
  rpc::Void           myReq;
  rpc::Table          myRep;

  ::uiiit::rpc::checkStatus(theStub->table(&myContext, myReq, &myRep));
  std::list<std::tuple<std::string, std::string, std::string>> myRet;
  for (const auto& elem : myRep.table()) {
    myRet.emplace_back(
        std::make_tuple(elem.client(), elem.appname(), elem.server()));
  }
  return myRet;
}

} // namespace etsimec
} // namespace uiiit
