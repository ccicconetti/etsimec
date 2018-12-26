#include "notificationevent.h"

#include <stdexcept>

namespace uiiit {
namespace etsimec {

NotificationEvent::NotificationEvent(const std::string& aUri)

    : rest::DataType() {
  if (aUri.empty()) {
    throw std::runtime_error("Invalid NotificationEvent: empty referenceURI");
  }
  theObject["referenceURI"] = web::json::value(aUri);
}

} // namespace etsimec
} // namespace uiiit
