#pragma once

#include "Rest/datatype.h"

namespace uiiit {
namespace etsimec {

/**
 * ETSI MEC NotificationEvent type.
 *
 * See ETSI GS MEC 016 v1.1.1 (2017-09).
 */
class NotificationEvent : public rest::DataType
{
 public:
  /**
   * \throw std::runtime_error if aUri is empty.
   */
  explicit NotificationEvent(const std::string& aUri);
};

} // namespace etsimec
} // namespace uiiit
