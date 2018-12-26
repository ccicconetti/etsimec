#pragma once

#include "EtsiMec/staticueapplcmproxy.h"

namespace uiiit {
namespace etsimec {

/**
 * A static UE application LCM proxy that reads the association between
 * addresses and egde routers from a text file, formatted as:
 *
 * address1 edgerouter1
 * address2 edgerouter2
 * ...
 * addressN edgerouterN
 *
 * The association above cannot be modified at run-time, unless the interface of
 * the base class is used. There is no default edge router.
 */
class StaticFileUeAppLcmProxy : public StaticUeAppLcmProxy
{
 public:
  /**
   * \param aApiRoot The apiRoot assigned.
   * \param aConfFile The input configuration file.
   *
   * \throw std::runtime_error if the configuration file is ill-formed or
   * unreadable or it contains no associations.
   */
  explicit StaticFileUeAppLcmProxy(const std::string& aApiRoot,
                                   const std::string& aConfFile);
};

} // namespace etsimec
} // namespace uiiit
