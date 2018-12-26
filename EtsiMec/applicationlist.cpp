#include "applicationlist.h"

namespace uiiit {
namespace etsimec {

ApplicationList::ApplicationList(const std::vector<AppInfo>& aAppInfos)
    : rest::DataType() {
  theObject["appInfo"] = web::json::value::array(aAppInfos.size());
  auto& myArray        = theObject["appInfo"].as_array();
  for (size_t i = 0; i < aAppInfos.size(); i++) {
    myArray.at(i) = aAppInfos.at(i).toJson();
  }
}

ApplicationList::ApplicationList(
    const std::map<std::string, AppInfo>& aAppInfos)
    : rest::DataType() {
  theObject["appInfo"] = web::json::value::array(aAppInfos.size());
  size_t i             = 0;
  auto&  myArray       = theObject["appInfo"].as_array();
  for (const auto& elem : aAppInfos) {
    myArray.at(i) = elem.second.toJson();
    i++;
  }
}

} // namespace etsimec
} // namespace uiiit
