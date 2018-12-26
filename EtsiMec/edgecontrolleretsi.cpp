#include "edgecontrolleretsi.h"

namespace uiiit {
namespace etsimec {

EdgeControllerEtsi::EdgeControllerEtsi(UeAppLcmProxy& aUeAppLcmProxy)
    : uiiit::edge::EdgeController()
    , theUeAppLcmProxy(aUeAppLcmProxy) {
}

void EdgeControllerEtsi::addLambda(const std::string& aLambda) {
  theUeAppLcmProxy.addApp(make(aLambda));
}

void EdgeControllerEtsi::delLambda(const std::string& aLambda) {
  theUeAppLcmProxy.delApp(make(aLambda));
}

AppInfo EdgeControllerEtsi::make(const std::string& aLambda) {
  return AppInfo(aLambda, "OpenLambdaMec", "1.0", "", AppCharcs());
}

} // namespace etsimec
} // namespace uiiit
