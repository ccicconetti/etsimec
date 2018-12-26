#pragma once

#include "Rest/server.h"

namespace uiiit {
namespace etsimec {

/**
 * A generic ETSI MEC server.
 */
class EtsiMecServer : public rest::Server
{
 public:
  virtual const std::string& apiName() const noexcept    = 0;
  virtual const std::string& apiVersion() const noexcept = 0;

 protected:
  explicit EtsiMecServer(const std::string& aApiRoot,
                         const std::string& aApiName,
                         const std::string& aApiVersion)
      : Server(aApiRoot + "/" + aApiName + "/" + aApiVersion + "/") {
  }
};

} // namespace etsimec
} // namespace uiiit
