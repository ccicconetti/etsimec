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
  std::ignore = aRep;
  VLOG(1) << "request to associate " << aReq->client() << " to "
          << aReq->server() << " from " << aContext->peer();

  theProxy.associateAddress(aReq->client(), aReq->server());
  return grpc::Status::OK;
}

grpc::Status GrpcUeAppLcmProxy::GrpcUeAppLcmProxyImpl::defaultEdgeRouter(
    grpc::ServerContext*     aContext,
    const rpc::AddressTuple* aReq,
    rpc::Void*               aRep) {
  assert(aContext);
  assert(aReq);
  assert(aReq->client().empty());
  std::ignore = aRep;
  VLOG(1) << "request to set default router to " << aReq->server() << " from "
          << aContext->peer();

  theProxy.defaultEdgeRouter(aReq->server());
  return grpc::Status::OK;
}

grpc::Status GrpcUeAppLcmProxy::GrpcUeAppLcmProxyImpl::removeAddress(
    grpc::ServerContext*     aContext,
    const rpc::AddressTuple* aReq,
    rpc::Void*               aRep) {
  assert(aContext);
  assert(aReq);
  assert(aReq->server().empty());
  std::ignore = aRep;
  VLOG(1) << "request to remove association of " << aReq->client() << " from "
          << aContext->peer();

  theProxy.removeAddress(aReq->client());
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

  const auto myTable = theProxy.addressAssociations();
  aRep->mutable_values()->insert(myTable.begin(), myTable.end());
  return grpc::Status::OK;
}

GrpcUeAppLcmProxy::GrpcUeAppLcmProxy(const std::string&   aServerEndpoint,
                                     StaticUeAppLcmProxy& aProxy)
    : SimpleServer(aServerEndpoint)
    , theServerImpl(aProxy) {
}

} // namespace etsimec
} // namespace uiiit
