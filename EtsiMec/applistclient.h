#pragma once

#include "EtsiMec/appinfo.h"

#include <list>
#include <memory>
#include <string>

namespace uiiit {
namespace etsimec {

/**
 * Query an ETSI UE application LCM proxy on the Mx2 interface to retrieve the
 * list of UE applications supported.
 */
class AppListClient final
{
 public:
  using ListType = std::list<AppInfo>;

  //! \param aApiRoot The URI of the ETSI UE application LCM proxy.
  explicit AppListClient(const std::string& aApiRoot);

  /**
   * \return the list of UE applications supported.
   *
   * \throw std::runtime_error if the proxy cannot be contacted or the GET
   * command raises any error.
   */
  ListType operator()() const;

 private:
  const std::string theApiRoot;
};

} // namespace etsimec
} // namespace uiiit
