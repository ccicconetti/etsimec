#include "utils.h"

#include "EtsiMec/ueapplcmproxy.h"

namespace uiiit {
namespace etsimec {

std::string removeTrailingSlashes(const std::string& aValue) {
  auto ret(aValue);
  while (not ret.empty() and ret.back() == '/') {
    ret.pop_back();
  }
  return ret;
}

std::string mx2root(const std::string& aApiRoot) {
  return removeTrailingSlashes(aApiRoot) + "/" +
         UeAppLcmProxy::staticApiName() + "/" +
         UeAppLcmProxy::staticApiVersion();
}

web::json::value parseJson(web::http::http_request aReq) {
  web::json::value myJson;
  aReq.extract_json()
      .then([&myJson](pplx::task<web::json::value> aPrevTask) {
        myJson = aPrevTask.get();
      })
      .wait();
  return myJson;
}

} // namespace etsimec
} // namespace uiiit
