#pragma once

#include "EtsiMec/appinfo.h"
#include "Rest/datatype.h"

#include <map>
#include <string>
#include <vector>

namespace uiiit {
namespace etsimec {

/**
 * ETSI MEC application list.
 *
 * See ETSI GS MEC 016 v1.1.1 (2017-09).
 */
class ApplicationList : public rest::DataType
{
 public:
  //! Create an application list from a vector.
  explicit ApplicationList(const std::vector<AppInfo>& aAppInfos);

  //! Create an application list from a map. Ignore keys.
  explicit ApplicationList(const std::map<std::string, AppInfo>& aAppInfos);
};

} // namespace etsimec
} // namespace uiiit
