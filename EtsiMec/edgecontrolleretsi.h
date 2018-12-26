#pragma once

#include "Edge/edgecontroller.h"
#include "EtsiMec/appinfo.h"
#include "EtsiMec/ueapplcmproxy.h"

namespace uiiit {
namespace etsimec {

/**
 * Convey notifications from an EdgeController to an ETSI UE application LCM
 * proxy.
 *
 * Fill the AppInfo structure as follows:
 * - name: lambda name
 * - provider: OpenLambdaMec
 * - version: 1.0
 * - description: <empty>
 * - application characteristics: <default>
 */
class EdgeControllerEtsi final : public uiiit::edge::EdgeController
{
 public:
  explicit EdgeControllerEtsi(UeAppLcmProxy& aUeAppLcmProxy);

 private:
  bool changeRoutes(
      const std::string&,
      const std::list<std::tuple<std::string, std::string, float>>&) override {
    return true;
  }
  bool removeRoutes(const std::string&,
                    const std::string&,
                    const std::list<std::string>&) override {
    return true;
  }

  void addLambda(const std::string& aLambda) override;

  void delLambda(const std::string& aLambda) override;

  static AppInfo make(const std::string& aLambda);

 private:
  UeAppLcmProxy& theUeAppLcmProxy;
};

} // namespace etsimec
} // namespace uiiit
