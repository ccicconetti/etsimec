#include "staticfileueapplcmproxy.h"

#include <fstream>

namespace uiiit {
namespace etsimec {

StaticFileUeAppLcmProxy::StaticFileUeAppLcmProxy(const std::string& aApiRoot,
                                                 const std::string& aConfFile)
    : StaticUeAppLcmProxy(aApiRoot) {
  std::ifstream myInput(aConfFile);
  if (not myInput) {
    throw std::runtime_error("Cannot open " + aConfFile + " for reading");
  }

  auto myEmpty = true;
  while (not myInput.eof()) {
    std::string myAddress;
    std::string myEdgeRouter;

    myInput >> myAddress;
    if (not myInput) {
      break;
    }

    myInput >> myEdgeRouter;
    if (not myInput) {
      throw std::runtime_error("Configuration file " + aConfFile +
                               " is ill-formed");
    }

    assert(not myAddress.empty() and not myEdgeRouter.empty());

    myEmpty = false;
    associateAddress(myAddress, myEdgeRouter);
  }

  if (myEmpty) {
    throw std::runtime_error("Empty configuration in " + aConfFile);
  }
}

} // namespace etsimec
} // namespace uiiit
