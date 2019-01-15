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

#include "grpcueapplcmproxy.h"

#include "EtsiMec/staticueapplcmproxy.h"

#include <glog/logging.h>
#include <grpc++/grpc++.h>

#include <cassert>

#define CATCH_ALL(__instruction__)                                             \
  try {                                                                        \
    __instruction__;                                                           \
  } catch (...) {                                                              \
  }

namespace uiiit {
namespace etsimec {

GrpcUeAppLcmProxy::GrpcUeAppLcmProxyImpl::GrpcUeAppLcmProxyImpl(
    StaticUeAppLcmProxy& aProxy)
    : theProxy(aProxy) {
}

grpc::Status GrpcUeAppLcmProxy::GrpcUeAppLcmProxyImpl::associateAddress(
    grpc::ServerContext*     aContext,
    const rpc::AddressTuple* aReq,
    rpc::Void*               aRep) {
  assert(aContext);
  assert(aReq);
  assert(not aReq->appname().empty());
  std::ignore = aRep;
  VLOG(1) << "request to associate <" << aReq->client() << ", "
          << aReq->appname() << "> to " << aReq->server() << " from "
          << aContext->peer();

  CATCH_ALL(theProxy.associateAddress(
      aReq->client(), aReq->appname(), aReq->server()));
  return grpc::Status::OK;
}

grpc::Status GrpcUeAppLcmProxy::GrpcUeAppLcmProxyImpl::removeAddress(
    grpc::ServerContext*     aContext,
    const rpc::AddressTuple* aReq,
    rpc::Void*               aRep) {
  assert(aContext);
  assert(aReq);
  assert(not aReq->appname().empty());
  assert(aReq->server().empty());
  std::ignore = aRep;
  VLOG(1) << "request to remove association of <" << aReq->client() << ", "
          << aReq->appname() << "> from " << aContext->peer();

  CATCH_ALL(theProxy.removeAddress(aReq->client(), aReq->appname()));
  return grpc::Status::OK;
}

grpc::Status GrpcUeAppLcmProxy::GrpcUeAppLcmProxyImpl::addLambda(
    grpc::ServerContext* aContext, const rpc::Lambda* aReq, rpc::Void* aRep) {
  assert(aContext);
  assert(aReq);
  assert(not aReq->value().empty());
  std::ignore = aRep;

  if (aReq->value().empty()) {
    LOG(ERROR) << "cannot add a lambda function with no name";

  } else {
    VLOG(1) << "request to add lambda function " << aReq->value() << " from "
            << aContext->peer();

    CATCH_ALL(theProxy.addApp(
        AppInfo(aReq->value(), "OpenLambdaMec", "1.0", "", AppCharcs())));
  }
  return grpc::Status::OK;
}

grpc::Status GrpcUeAppLcmProxy::GrpcUeAppLcmProxyImpl::delLambda(
    grpc::ServerContext* aContext, const rpc::Lambda* aReq, rpc::Void* aRep) {
  assert(aContext);
  assert(aReq);
  assert(not aReq->value().empty());
  std::ignore = aRep;

  if (aReq->value().empty()) {
    LOG(ERROR) << "cannot remove a lambda function with no name";

  } else {
    VLOG(1) << "request to remove lambda function " << aReq->value() << " from "
            << aContext->peer();

    CATCH_ALL(theProxy.delApp(etsimec::AppInfo(
        aReq->value(), "OpenLambdaMec", "1.0", "", etsimec::AppCharcs())));
  }
  return grpc::Status::OK;
}

grpc::Status GrpcUeAppLcmProxy::GrpcUeAppLcmProxyImpl::numContexts(
    grpc::ServerContext* aContext, const rpc::Void* aReq, rpc::Number* aRep) {
  assert(aContext);
  assert(aRep);
  std::ignore = aReq;
  VLOG(2) << "request to retrieve number of contexts from " << aContext->peer();

  aRep->set_value(theProxy.numContexts());
  return grpc::Status::OK;
}

grpc::Status GrpcUeAppLcmProxy::GrpcUeAppLcmProxyImpl::table(
    grpc::ServerContext* aContext, const rpc::Void* aReq, rpc::Table* aRep) {
  assert(aContext);
  assert(aRep);
  std::ignore = aReq;
  VLOG(2) << "request to retrieve association from " << aContext->peer();

  for (const auto& elem : theProxy.addressAssociations()) {
    auto myTuple = aRep->add_table();
    myTuple->set_client(std::get<0>(elem));
    myTuple->set_appname(std::get<1>(elem));
    myTuple->set_server(std::get<2>(elem));
  }
  return grpc::Status::OK;
}

GrpcUeAppLcmProxy::GrpcUeAppLcmProxy(const std::string&   aServerEndpoint,
                                     StaticUeAppLcmProxy& aProxy)
    : SimpleServer(aServerEndpoint)
    , theServerImpl(aProxy) {
}

} // namespace etsimec
} // namespace uiiit
