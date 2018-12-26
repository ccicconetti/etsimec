#include "appcharcs.h"

#include "Rest/utils.h"
#include "Support/thrower.h"

namespace uiiit {
namespace etsimec {

AppCharcs::AppCharcs(const unsigned int aMemory,
                     const unsigned int aStorage,
                     const unsigned int aLatency,
                     const unsigned int aBandwidth,
                     const ServiceCont  aServiceCont)
    : rest::DataType() {
  theObject["memory"]      = web::json::value(aMemory);
  theObject["storage"]     = web::json::value(aStorage);
  theObject["latency"]     = web::json::value(aLatency);
  theObject["bandwidth"]   = web::json::value(aBandwidth);
  theObject["serviceCont"] = web::json::value(static_cast<int>(aServiceCont));
}

AppCharcs::AppCharcs()
    : AppCharcs(0, 0, 0, 0, ServiceCont::NOT_REQUIRED) {
}

AppCharcs::AppCharcs(const web::json::value& aJson)
    : DataType(aJson) {
  support::Thrower myThrower("Invalid AppCharcs: ");
  rest::notNumberIfPresent(aJson, "memory", myThrower);
  rest::notNumberIfPresent(aJson, "storage", myThrower);
  rest::notNumberIfPresent(aJson, "latency", myThrower);
  rest::notNumberIfPresent(aJson, "bandwidth", myThrower);
  rest::notNumberIfPresent(aJson, "serviceCont", myThrower);
  if (aJson.has_number_field("serviceCont")) {
    const auto myServiceCont = aJson.at("serviceCont").as_integer();
    myThrower(myServiceCont != static_cast<int>(ServiceCont::NOT_REQUIRED) and
                  myServiceCont != static_cast<int>(ServiceCont::REQUIRED),
              "invalid serviceCont value");
  }
  myThrower.check();
}

} // namespace etsimec
} // namespace uiiit
