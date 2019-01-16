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

#pragma once

#include "RpcSupport/simpleserver.h"

#include <string>

#include "ueapplcmproxy.grpc.pb.h"

namespace uiiit {
namespace etsimec {

class StaticUeAppLcmProxy;

/**
 * A gRPC server interface to a StaticUeAppLcmProxy.
 */
class GrpcUeAppLcmProxy final : public ::uiiit::rpc::SimpleServer
{
  class GrpcUeAppLcmProxyImpl final : public rpc::UeAppLcmProxy::Service
  {
   public:
    explicit GrpcUeAppLcmProxyImpl(StaticUeAppLcmProxy& aServer);

   private:
    grpc::Status associateAddress(grpc::ServerContext*     aContext,
                                  const rpc::AddressTuple* aReq,
                                  rpc::Void*               aRep) override;
    grpc::Status removeAddress(grpc::ServerContext*     aContext,
                               const rpc::AddressTuple* aReq,
                               rpc::Void*               aRep) override;
    grpc::Status addLambda(grpc::ServerContext*     aContext,
                               const rpc::Lambda* aReq,
                               rpc::Void*               aRep) override;
    grpc::Status delLambda(grpc::ServerContext*     aContext,
                               const rpc::Lambda* aReq,
                               rpc::Void*               aRep) override;
    grpc::Status numContexts(grpc::ServerContext* aContext,
                             const rpc::Void*     aReq,
                             rpc::Number*         aRep) override;
    grpc::Status table(grpc::ServerContext* aContext,
                       const rpc::Void*     aReq,
                       rpc::Table*          aRep) override;
    grpc::Status contexts(grpc::ServerContext* aContext,
                       const rpc::Void*     aReq,
                       rpc::Contexts*          aRep) override;

   private:
    StaticUeAppLcmProxy& theProxy;
  };

 public:
  /**
   * \param aServerEndpoint the gRPC end-point exposed.
   * \param aProxy the UE application LCM proxy.
   */
  explicit GrpcUeAppLcmProxy(const std::string&   aServerEndpoint,
                             StaticUeAppLcmProxy& aProxy);

 private:
  grpc::Service& service() override {
    return theServerImpl;
  }

 private:
  GrpcUeAppLcmProxyImpl theServerImpl;
};

} // namespace etsimec
} // namespace uiiit
