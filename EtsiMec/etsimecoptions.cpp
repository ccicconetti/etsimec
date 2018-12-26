#include "EtsiMec/etsimecoptions.h"

namespace uiiit {
namespace etsimec {

EtsiMecOptions::EtsiMecOptions(
    int argc, char** argv, boost::program_options::options_description& aDesc)
    : support::CliOptions(argc, argv, aDesc)
    , theLcmRoot() {
  // clang-format off
  theDesc.add_options()
  ("etsi-ue-app-lcm-proxy-root,r",
   boost::program_options::value<std::string>(&theLcmRoot)->default_value(""),
   "API root of the ETSI UE application LCM proxy, use --server-endpoint if empty.")
  ;
  // clang-format on
  parse();
}

const std::string& EtsiMecOptions::lcmRoot() const noexcept {
  return theLcmRoot;
}

} // namespace etsimec
} // namespace uiiit
