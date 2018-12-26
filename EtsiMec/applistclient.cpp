#include "applistclient.h"

#include "EtsiMec/utils.h"
#include "Rest/client.h"

namespace uiiit {
namespace etsimec {

AppListClient::AppListClient(const std::string& aApiRoot)
    : theApiRoot(aApiRoot) {
}

AppListClient::ListType AppListClient::operator()() const {
  rest::Client myClient(etsimec::mx2root(theApiRoot));
  const auto   ret = myClient.get("/app_list");
  if (ret.first != web::http::status_codes::OK or
      not ret.second.has_object_field("ApplicationList") or
      not ret.second.at("ApplicationList").has_array_field("appInfo")) {
    throw std::runtime_error("Invalid response from server: " +
                             std::to_string(ret.first));
  }

  ListType myApplications;
  for (const auto& elem :
       ret.second.at("ApplicationList").at("appInfo").as_array()) {
    myApplications.emplace_back(AppInfo(elem));
  }
  return myApplications;
}

} // namespace etsimec
} // namespace uiiit
