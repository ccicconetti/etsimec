#pragma once

#include "Rest/datatype.h"

namespace uiiit {
namespace etsimec {

/**
 * ETSI MEC application characteristics.
 *
 * See ETSI GS MEC 016 v1.1.1 (2017-09).
 */
class AppCharcs : public rest::DataType
{
 public:
  enum class ServiceCont : int {
    NOT_REQUIRED = 0,
    REQUIRED     = 1,
  };

  explicit AppCharcs(const unsigned int aMemory,
                     const unsigned int aStorage,
                     const unsigned int aLatency,
                     const unsigned int aBandwidth,
                     const ServiceCont  aServiceCont);

  //! A set of empty application characteristics.
  explicit AppCharcs();

  /**
   * Build from JSON.
   *
   * \throw std::runtime_error if invalid.
   */
  explicit AppCharcs(const web::json::value& aJson);
};

} // namespace etsimec
} // namespace uiiit
