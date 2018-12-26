#pragma once

#include "Support/clioptions.h"

namespace uiiit {
namespace etsimec {

/**
 * Specialization of CLI options for ETSI MEC applications.
 */
class EtsiMecOptions final : public support::CliOptions
{
 public:
  EtsiMecOptions(int                                          argc,
                 char**                                       argv,
                 boost::program_options::options_description& aDesc);

  //! \return the UE app LCM proxy root.
  const std::string& lcmRoot() const noexcept;

 private:
  std::string theLcmRoot;
};

} // namespace etsimec
} // namespace uiiit
